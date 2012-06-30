#include <QFile>
#include <QRegExp>
#include <QTimerEvent>
#include "Server.h"

const int PROTOCOL_VERSION = Server::PV_BETA;

Server::Server( QObject* parent ):
    QObject( parent ),
    tcpServer_( new QTcpServer(this) ),
    guestAllowed_( false ),
    address_( DEFAULT_SERVER_ADDRESS ),
    port_( DEFAULT_SERVER_PORT )
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

    qDebug(
        "Server spawned at %s:%d",
        qPrintable(address.toString()),
        port
    );

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

void Server::timerEvent( QTimerEvent* event )
{
    Q_UNUSED( event );
    // Searching for free clients and connecting them
    ClientsIterator freeClient = clients_.end();

    for( ClientsIterator cit = clients_.begin(); cit != clients_.end(); cit++ )
    {
        if( cit->status != Client::ST_READY )
            continue;

        if( freeClient == clients_.end() )
        {
            freeClient = cit;
            continue;
        }

        connectTwoClients( freeClient, cit );
        freeClient = clients_.end();
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
    /*
    client1->socket->write( "found:\n" );
    client2->socket->write( "found:\n" );
    client1->socket->write( "go:\n" );
    */
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

    if( cit == clients_.end() )
    {
        qDebug() << "No users";
        return;
    }

    if( stateAuthorize(cmd, cit) )
        return;

    if( stateRecieveField(cmd, cit) )
        return;

    if( stateRecieveSteps(cmd, cit) )
        return;

    if( stateRecieveStatus(cmd, cit) )
        return;

    cit->send( "wrongcmd:" );
}

bool Server::stateAuthorize( const QString& cmd, ClientsIterator client )
{
    QRegExp rx( "mbclient:(\\d+):((\\w|\\d){3,16}):(.+):" );
    if( rx.indexIn(cmd) == -1 )
        return false;

    if( client->status == Client::ST_AUTHORIZED && client->login == rx.cap(2) )
    {
        client->send( "alreadyauth:" );
        return true;
    }

    if( !checkProtocolVersion(rx.cap(1).toInt()) )
    {
        client->send( "wrongver:" );
        disconnectClient( client );
        return true;
    }

    if( !checkUserLogin(rx.cap(2), rx.cap(4)) )
    {
        client->send( "wronguser:" );
        disconnectClient( client );
        return true;
    }

    client->status = Client::ST_AUTHORIZED;
    client->login = rx.cap(2);
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

    return true;
}

bool Server::stateRecieveSteps( const QString& cmd, ClientsIterator client )
{

    return true;
}

bool Server::stateRecieveStatus( const QString& cmd, ClientsIterator client )
{
    QRegExp rx( "disconnect:" );
    if( rx.indexIn(cmd) == -1 )
        return false;

    disconnectClient( client );
    qDebug() << "User" << client->login << "is disconnected";
    return true;
}

void Server::disconnectClient( ClientsIterator client )
{
    client->socket->disconnectFromHost();
    int cid = client->socket->socketDescriptor();

    if( clients_.find(cid) != clients_.end() )
        clients_.remove( cid );
}

bool Server::checkProtocolVersion( int version )
{
    return version == PROTOCOL_VERSION;
}

bool Server::checkUserLogin( const QString& login, const QString& password )
{
    if( login == "guest" )
        return true;

    if( !QFile::exists(authFile_) )
    {
        qWarning() << "WARNING: Auth file not exists";
        return false;
    }

    QFile af( authFile_ );

    if( !af.open(QFile::ReadOnly) )
    {
        qCritical() << "ERROR: Unable to open auth file";
        return false;
    }

    af.close();

    return true;
}
