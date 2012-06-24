#include "MainServer.h"

void Client::send( const QString& cmd )
{
    QTextStream clientStream( socket );
    clientStream << cmd << "\n";
    clientStream << flush;
}

MainServer::MainServer():
    server( NULL ),
    timer( NULL ),
    address( QHostAddress::Any ),
    port( DEFAULT_PORT )
{
    server = new QTcpServer( this );
    connect(
        server,
        SIGNAL( newConnection() ),
        this,
        SLOT( onNewUserConnected() )
    );
}

void MainServer::parceCmdLine( const QStringList& arguments )
{
    for( int i = 1; i < arguments.count(); i++ )
    {
        if(
            arguments.at(i).compare("--port") == 0 &&
            i < arguments.count() - 1
        )
        {
            bool ok;
            this->port = arguments.at( i + 1 ).toInt( &ok );

            if( !ok )
            {
                this->port = DEFAULT_PORT;
                continue;
            }

            i++;
            continue;
        }

        if(
            arguments.at(i).compare("--address") == 0 &&
            i < arguments.count() - 1
        )
        {
            this->address.setAddress( arguments.at(i+1) );
            i++;
            continue;
        }
    }
}

bool MainServer::spawn()
{
    return spawn( this->address, this->port );
}

bool MainServer::spawn( const QHostAddress& address, quint16 port )
{
    if( !server )
        return false;

    if( !server->listen( address, port ) )
    {
        qDebug(
            "Server spawning failed: %s",
            qPrintable( server->errorString() )
        );
        return false;
    }

    timer = new QTimer( this );
    timer->setInterval( DEFAULT_SEARCH_INTERVAL );
    connect( timer, SIGNAL(timeout()), this, SLOT(onTimer()) );
    timer->start();

    qDebug(
        "Server started at %s:%d",
        qPrintable( address.toString() ),
        port
    );
    return true;
}

void MainServer::onNewUserConnected()
{
    Client client;
    client.socket = server->nextPendingConnection();
    client.status = ST_CONNECTED;
    client.playingWith = clients.end();
    int clientId = client.socket->socketDescriptor();
    clients.insert( clientId, client );
    connect(
        client.socket,
        SIGNAL( readyRead() ),
        this,
        SLOT( receivedData() )
    );
    qDebug() << "Client connected";
}

void MainServer::receivedData()
{
    QTcpSocket* clientSocket = ( QTcpSocket* )sender();
    QByteArray data = clientSocket->readAll();
    parseData( data, clientSocket->socketDescriptor() );
}

void MainServer::parseData( const QString& cmd, int clientId )
{
    Clients::iterator i = clients.find( clientId );

    if( i == clients.end() )
        return;

    if( authorize( cmd, i ) )
        return;

    if( setField( cmd, i ) )
        return;

    if( makeStep( cmd, i ) )
        return;

    i->send( "wrongcmd:" );
}

bool MainServer::authorize( const QString& cmd, Clients::iterator client )
{
    // Checking received data for authorization and making authorization
    QRegExp rx( "mbclient:(\\d+):(\\w+):(.+):" );

    if( rx.indexIn( cmd ) != -1 )
    {
        if( client->status == ST_CONNECTED )
        {
            // TODO: check for version
            qDebug() << "Version: " << rx.cap( 1 );

            qDebug() << "Login: " << rx.cap( 2 );
            qDebug() << "Password: " << rx.cap( 3 );
            client->status = ST_AUTHORIZED;

            // TODO: take version from defines
            client->send( "mbserver:1:" );
            return true;
        }
    }

    return false;
}

bool MainServer::setField( const QString& cmd, Clients::iterator client )
{
    QRegExp rx( "field:([01]+):" );

    if( rx.indexIn( cmd ) != -1 )
    {
        if( client->status == ST_AUTHORIZED )
        {
            qDebug() << "Field: " << rx.cap( 1 );

            if( !placeShips( rx.cap( 1 ), client ) )
                return false;

            qDebug() << "Len=100";
            client->status = ST_READY;
            return true;
        }
    }

    return false;
}

bool MainServer::makeStep( const QString& cmd, Clients::iterator client )
{
    QRegExp rx( "step:(\\d):(\\d):" );

    if( rx.indexIn( cmd ) != -1 )
    {
        if( client->status == ST_MAKING_STEP )
        {
            int x = rx.cap( 1 ).toInt();
            int y = rx.cap( 2 ).toInt();
            QString response1, response2;
            Cell current = client->playingWith->field.getCell( x, y );

            if( current == CL_CLEAR || current == CL_DOT )
            {
                current = CL_DOT;
                response1 = QString( "field2:miss:%1:%2:" ).arg( x ).arg( y );
                response2 = QString( "field1:miss:%1:%2:" ).arg( x ).arg( y );
            }
            else
            {
                current = CL_HALF;
                response1 = QString( "field2:half:%1:%2:" ).arg( x ).arg( y );
                response2 = QString( "field1:half:%1:%2:" ).arg( x ).arg( y );
                // TODO: check for kill
            }

            client->status = ST_WAITING_STEP;
            client->playingWith->status = ST_MAKING_STEP;
            client->send( response1 );
            client->playingWith->send( response2 );
            // TODO: check who's step
            client->playingWith->send( "go:" );
            qDebug( "Making step" );
            return true;
        }
    }

    return false;
}

bool MainServer::placeShips( const QString& ships, Clients::iterator client )
{
    if( !Field::isFieldCorrect(ships) )
        return false;

    client->field.initField( ships );
    return true;
}

void MainServer::onTimer()
{
    // Searching for free clients and connecting them
    Clients::iterator freeClient = clients.end();

    for( Clients::iterator i = clients.begin(); i != clients.end(); i++ )
    {
        if( i->status == ST_READY )
        {
            if( freeClient == clients.end() )
                freeClient = i;
            else
            {
                connectTwoClients( freeClient, i );
                freeClient = clients.end();
            }
        }
    }
}

void MainServer::connectTwoClients(
    Clients::iterator client1,
    Clients::iterator client2
)
{
    client1->status = ST_MAKING_STEP;
    client2->status = ST_WAITING_STEP;
    client1->playingWith = client2;
    client2->playingWith = client1;
    client1->socket->write( "found:\n" );
    client2->socket->write( "found:\n" );
    client1->socket->flush();
    SleeperThread sleeper;
    sleeper.msleep( 100 );
    client1->socket->write( "go:\n" );
}
