#pragma once

#include <QObject>
#include <QHostAddress>
#include <QtNetwork/QTcpServer>
#include "constants.h"
#include "Client.h"

class Server : public QObject
{
    Q_OBJECT
public:
    enum ProtocolVersion
    {
        PV_ALPHA = 1,
        PV_BETA,
        PV_RELEASE
    };

    explicit Server( QObject* parent = 0 );
    ~Server();

    bool spawn();
    bool spawn( const QString& address, quint16 port = DEFAULT_SERVER_PORT );
    bool spawn(
        const QHostAddress& address,
        quint16 port = DEFAULT_SERVER_PORT
    );

    void setGuestAllowed( bool state );
    bool isGuestAllowed() const;
    void setAddress( const QString& address );
    QString address() const;
    void address( QHostAddress& addr ) const;
    void setPort( quint16 port );
    int port() const;
    void setAuthFile( const QString& filename );
    const QString& authFile() const;
    void setStatFile( const QString& filename );
    const QString& statFile() const;

private slots:
    void on_newUserConnected();
    void on_recievedData();

protected:
    void timerEvent( QTimerEvent* event );

private:
    void connectTwoClients(
        Clients::iterator client1,
        Clients::iterator client2
    );
    void disconnectClient( ClientsIterator client );
    void parseData( const QString& cmd, int clientId );
    bool stateAuthorize( const QString& cmd, ClientsIterator client );
    bool stateRecieveField( const QString& cmd, ClientsIterator client );
    bool stateRecieveSteps( const QString& cmd, ClientsIterator client );
    bool stateRecieveStatus( const QString& cmd, ClientsIterator client );
    bool checkProtocolVersion( int version );
    bool checkUserLogin( const QString& login, const QString& password );

private:
    QTcpServer* tcpServer_;
    bool guestAllowed_;
    QHostAddress address_;
    quint16 port_;
    QString authFile_;
    QString statFile_;
    Clients clients_;
};
