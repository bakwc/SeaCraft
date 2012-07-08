#include <QFile>
#include <QRegExp>
#include <QTimerEvent>
#include "Server.h"

const int PROTOCOL_VERSION = Server::PV_BETA;

Server::Server( QObject* parent ):
    QObject( parent ),
    tcpServer_( new QTcpServer(this) ),
    spawned_( false ),
    guestAllowed_( false ),
    registrationAllowed_( true ),
    address_( DEFAULT_SERVER_ADDRESS ),
    port_( DEFAULT_SERVER_PORT ),
    authFile_( DEFAULT_AUTH_FILE ),
    statFile_( DEFAULT_STAT_FILE )
{
    connect(
        tcpServer_,
        SIGNAL(newConnection()),
        this,
        SLOT(on_newUserConnected())
    );
}

Server::~Server()
{
    if( spawned_ )
        stats_.save( statFile_ );

    spawned_ = false;
}

bool Server::spawn()
{
    return spawn( address(), port() );
}

bool Server::spawn( const QString& address, quint16 port )
{
    return spawn( QHostAddress(address), port );
}

bool Server::spawn( const QHostAddress& address, quint16 port )
{
    if( !tcpServer_ )
        return false;

    if( !tcpServer_->listen(address, port) )
    {
        qCritical(
            "Server::spawn(): server spawning failed: '%s'",
            qPrintable(tcpServer_->errorString())
        );
        return false;
    }

    startTimer( DEFAULT_SEARCH_INTERVAL );
    stats_.load( statFile_ );
    spawned_ = true;

    qDebug( "Server spawned at %s:%d", qPrintable(address.toString()), port );

    if( guestAllowed_ )
        qDebug( "Guest account enabled." );

    if( !registrationAllowed_ )
        qDebug( "User registration disabled." );

    return true;
}

void Server::setGuestAllowed( bool state )
{
    guestAllowed_ = state;
}

bool Server::isGuestAllowed() const
{
    return guestAllowed_;
}

void Server::setRegistrationAllowed( bool state )
{
    registrationAllowed_ = state;
}

bool Server::isRegistrationAllowed() const
{
    return registrationAllowed_;
}

void Server::setAddress( const QString& address )
{
    address_ = QHostAddress( address );
    if( address_ == QHostAddress::Null )
        address_ = DEFAULT_SERVER_ADDRESS;
}

QString Server::address() const
{
    return address_.toString();
}

void Server::address( QHostAddress& addr ) const
{
    addr = address();
}

void Server::setPort( quint16 port )
{
    port_ = port;
}

int Server::port() const
{
    return port_;
}

void Server::setAuthFile( const QString& filename )
{
    authFile_ = filename;
}

const QString& Server::authFile() const
{
    return authFile_;
}

void Server::setStatFile( const QString& filename )
{
    statFile_ = filename;
}

const QString& Server::statFile() const
{
    return statFile_;
}

// Searching for free clients and connecting them
// Also removing inactive clients
void Server::timerEvent( QTimerEvent* event )
{
    Q_UNUSED( event );
    ClientsIterator freeClient = clients_.end();

    for( ClientsIterator cit = clients_.begin(); cit != clients_.end(); cit++ )
    {
        // Remove disconnected clients from list
        if( cit->status == Client::ST_DISCONNECTED )
        {
            if( freeClient == cit )
                freeClient = clients_.end();

            cit = clients_.erase( cit );
            continue;
        }

        if( cit->lastSeen() >= DEFAULT_INACTIVE_MIN )
            cit->socket->write( "ping:" );

        // Mark zombie clients as disconnected and record to the statistic
        if( cit->lastSeen() >= DEFAULT_INACTIVE_MAX )
        {
            disconnectClientAndRecord( cit, false );
            continue;
        }

        if( cit->status != Client::ST_READY )
            continue;

        if(
            freeClient != clients_.end() &&
            freeClient != cit &&
            freeClient->status == Client::ST_READY
        )
        {
            connectTwoClients( freeClient, cit );
            freeClient = clients_.end();
            continue;
        }

        freeClient = cit;
    }
}

void Server::connectTwoClients(
    ClientsIterator client1,
    ClientsIterator client2
)
{
    client1->status = Client::ST_MAKING_STEP;
    client2->status = Client::ST_WAITING_STEP;
    client1->playingWith = client2;
    client2->playingWith = client1;
    client1->socket->write(
        qPrintable(
            QString("found:%1:").arg(client2->login)
        )
    );
    client2->socket->write(
        qPrintable(
            QString("found:%1:").arg(client1->login)
        )
    );
    client1->socket->write( "go:" );
}

void Server::on_newUserConnected()
{
    qDebug() << "Server::on_newUserConnected(): new anonymous user connected";

    Client client;
    client.socket = tcpServer_->nextPendingConnection();
    client.status = Client::ST_CONNECTED;
    client.playingWith = clients_.end();
    int clientId = client.socket->socketDescriptor();
    clients_.insert( clientId, client );
    connect(
        client.socket,
        SIGNAL(readyRead()),
        this,
        SLOT(on_recievedData())
    );
}

void Server::on_recievedData()
{
    QTcpSocket* clientSocket = ( QTcpSocket* )sender();
    QByteArray data = clientSocket->readAll();
    parseData( data, clientSocket->socketDescriptor() );
}

void Server::parseData( const QString& cmd, int clientId )
{
    ClientsIterator cit = clients_.find( clientId );

    cit->setSeen();

    if( cit == clients_.end() )
        return;

    if( stateAuthorize(cmd, cit) )
        return;

    if( stateRecieveField(cmd, cit) )
        return;

    if( stateRecieveSteps(cmd, cit) )
        return;

    if( stateRecieveStatus(cmd, cit) )
        return;

    if( stateRecievePing(cmd, cit) )
        return;

    cit->send( "wrongcmd:" );
}

bool Server::stateAuthorize( const QString& cmd, ClientsIterator client )
{
    QRegExp rx(
        QString("mbclient:(\\d+):((\\w|\\d){%1,%2}):(.+):")
            .arg(LOGIN_LENGTH_MIN)
            .arg(LOGIN_LENGTH_MAX)
    );
    if( rx.indexIn(cmd) == -1 )
        return false;

    const QString& login = rx.cap(2);
    const QString& password = rx.cap(4);


    if (isUserConnected(login))
    {
        client->send( "alreadyauth:" );
        disconnectClient( client );
        return true;
    }

    if( client->status == Client::ST_AUTHORIZED && client->login == login )
    {
        client->send( "alreadyauth:" );
        disconnectClient( client );
        return true;
    }

    if( !checkProtocolVersion(rx.cap(1).toInt()) )
    {
        client->send( "wrongver:" );
        disconnectClient( client );
        return true;
    }

    CheckUserStatus cus = checkUserLogin( login, password );
    if( registrationAllowed_ && cus == CUS_NOTFOUND )
    {
        cus = CUS_OK;
        if( !registerUserLogin(login, password) )
        {
            // TODO: return 'server error' status or smth
            return true;
        }
    }

    if( cus != CUS_OK || (!guestAllowed_ && login == DEFAULT_GUEST_ACCOUNT) )
    {
        client->send( "wronguser:" );
        disconnectClient( client );
        return true;
    }

    client->status = Client::ST_AUTHORIZED;
    client->login = login;
    client->playingWith = clients_.end();
    client->send(
        qPrintable(
            QString("mbserver:%1:").arg(PROTOCOL_VERSION)
        )
    );

    qDebug() << "User" << client->login << "authorized";
    return true;
}

bool Server::stateRecieveField( const QString& cmd, ClientsIterator client )
{
    QRegExp rx( "field:(\\d{1,2}):([0-8]+):" );
    if( rx.indexIn(cmd) == -1 )
        return false;

    if( client->status != Client::ST_AUTHORIZED )
        return false;

    int shipSize = rx.cap(1).toInt();
    const QString& field = rx.cap(2);

    client->setField( field, shipSize );
    if( !client->field()->checkField() )
    {
        client->send( "wrongfield:" );
        qDebug() << "User" << client->login << "passed wrong field";
        client->field()->showField();
        disconnectClient( client );
        return true;
    }

    client->status = Client::ST_READY;
    return true;
}

bool Server::stateRecievePing( const QString& cmd, ClientsIterator client )
{
    QRegExp rx( "pong:" );
    if( rx.indexIn(cmd) == -1 )
        return false;

    client->setSeen();
    return true;
}

bool Server::stateRecieveSteps( const QString& cmd, ClientsIterator client )
{
    QRegExp rx( "step:(\\d):(\\d):" );
    if( rx.indexIn(cmd) == -1 )
        return false;

    if( client->status != Client::ST_MAKING_STEP )
        return false;

    qDebug() << "User" << client->login << "is making step";

    quint32 x = rx.cap( 1 ).toInt();
    quint32 y = rx.cap( 2 ).toInt();

    QString response1, response2;
    Field* enemyField = client->playingWith->field();
    Field::Cell current = enemyField->getCell( x, y );

    if(
        (current != Field::CI_CLEAR && current > Field::CellShipsTypeCount) ||
        //x < 0 || y < 0 ||
        x >= enemyField->getFieldLength() ||
        y >= enemyField->getFieldLength()
    )
    {
        client->send( "wrongstep:" );
        return true;
    }

    Field::Shots killShots;
    bool isKilled = enemyField->makeShot( x, y, killShots );
    bool isMissed = killShots.isEmpty();
    QString type = isKilled ? "kill" : "half";
    Field::Cell cell = isMissed ? Field::CI_DOT : Field::CI_DAMAGED;

    if( isMissed )
    {
        enemyField->setCell( x, y, cell );
        client->status = Client::ST_WAITING_STEP;
        client->playingWith->status = Client::ST_MAKING_STEP;

        response1 = QString( "field2:miss:%1:%2:" ).arg( x ).arg( y );
        response2 = QString( "field1:miss:%1:%2:" ).arg( x ).arg( y );

        client->send( response1 );
        client->playingWith->send( response2 );
        client->playingWith->send( "go:" );
        return true;
    }

    for( int i = 0; i < killShots.size(); i++ )
    {
        response1 = QString( "field2:%1:%2:%3:" )
            .arg( type )
            .arg( killShots.at(i).x() )
            .arg( killShots.at(i).y() );
        response2 = QString( "field1:%1:%2:%3:" )
            .arg( type )
            .arg( killShots.at(i).x() )
            .arg( killShots.at(i).y() );

        client->send( response1 );
        client->playingWith->send( response2 );
    }

    enemyField->setCell( x, y, cell );

    if( enemyField->isAllKilled() )
    {
        disconnectClientAndRecord( client, true );
        return true;
    }

    client->send( "go:" );
    return true;
}

bool Server::stateRecieveStatus( const QString& cmd, ClientsIterator client )
{
    QRegExp rx( "disconnect:" );
    if( rx.indexIn(cmd) == -1 )
        return false;

    disconnectClientAndRecord( client, false );
    return true;
}

bool Server::isUserConnected(const QString& login)
{
    for (ClientsIterator i=clients_.begin();i!=clients_.end();i++)
        if (i->login==login)
            return true;
    return false;
}



// Disconnect client from server
void Server::disconnectClient( ClientsIterator client )
{
    client->socket->disconnectFromHost();
    client->status = Client::ST_DISCONNECTED;
    if( client->playingWith != clients_.end() )
        client->playingWith->playingWith = clients_.end();
    client->playingWith = clients_.end();

    qDebug() << "User" << client->login << "is disconnected";
}

// Disconnect client from server and record winner status for him and his
// opponent, if there was a game between'em
void Server::disconnectClientAndRecord(
    ClientsIterator client,
    bool winnerStatus
)
{
    if( client == clients_.end() )
        return;

    if(
        client->status == Client::ST_MAKING_STEP ||
        client->status == Client::ST_WAITING_STEP
    )
    {
        Client& winner = winnerStatus
            ? client.value()
            : client->playingWith.value();
        Client& looser = !winnerStatus
            ? client.value()
            : client->playingWith.value();

        winner.send( "win:" );
        looser.send( "lose:" );

        recordSessionStatistic(
            winner.login,
            looser.login
        );
    }

    if( client->playingWith != clients_.end() )
        disconnectClient( client->playingWith );

    disconnectClient( client );
}

bool Server::checkProtocolVersion( int version )
{
    return version == PROTOCOL_VERSION;
}

Server::CheckUserStatus Server::checkUserLogin(
    const QString& login,
    const QString& password
)
{
    if( guestAllowed_ && login == DEFAULT_GUEST_ACCOUNT )
        return CUS_OK;

    if( !QFile::exists(authFile_) )
    {
        qWarning() << "WARNING: Auth file not exists";
        return CUS_NOTFOUND;
    }

    QFile af( authFile_ );

    if( !af.open(QFile::ReadOnly) )
    {
        qCritical() << "ERROR: Unable to open auth file";
        return CUS_NOTFOUND;
    }

    QByteArray data;
    QRegExp rx(
        QString("((\\d|\\w| ){%1,%2}):((\\d|\\w){%3,%4}):")
            .arg(LOGIN_LENGTH_MIN).arg(LOGIN_LENGTH_MAX)
            .arg(PASSWORD_LENGTH_MIN).arg(PASSWORD_LENGTH_MAX)
    );

    while( !af.atEnd() )
    {
        data = af.readLine();

        if( rx.indexIn(data) == -1 )
            continue;

        if( login.compare(rx.cap(1)) == 0 )
        {
            af.close();

            if( password.compare(rx.cap(3)) == 0 )
                return CUS_OK;

            return CUS_WRONGPASS;
        }
    }

    af.close();
    return CUS_NOTFOUND;
}

bool Server::registerUserLogin( const QString& login, const QString& password )
{
    QFile af( authFile_ );
    if( !af.open(QFile::Append) )
    {
        qDebug() << "Unable to open auth file";
        return false;
    }

    af.write( qPrintable(QString("%1:%2:\n").arg(login).arg(password)) );
    af.close();
    return true;
}

void Server::recordSessionStatistic(
    const QString& winner,
    const QString& looser
)
{
    // TODO: add additional checks
    stats_.playerWon( winner );
    stats_.playerLost( looser );

    stats_.save( statFile_ );
}
