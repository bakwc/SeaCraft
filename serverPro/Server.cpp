#include "Server.h"

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

void Server::on_newUserConnected()
{
    qDebug() << "Server::on_newUserConnected(): new user connected";
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
