#pragma once

#include <QObject>
#include <QHostAddress>
#include <QtNetwork/QTcpServer>
#include "Constants.h"
#include "Client.h"
#include "Statistics.h"

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

    enum CheckUserStatus
    {
        CUS_NOTFOUND = -1,
        CUS_OK = 0,
        CUS_WRONGPASS = 1
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
    void setRegistrationAllowed( bool state );
    bool isRegistrationAllowed() const;
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
    void disconnectClientAndRecord( ClientsIterator client, bool winnerStatus );
    void parseData( const QString& cmd, int clientId );
    bool stateAuthorize( const QString& cmd, ClientsIterator client );
    bool stateRecieveField( const QString& cmd, ClientsIterator client );
    bool stateRecievePing( const QString& cmd, ClientsIterator client );
    bool stateRecieveSteps( const QString& cmd, ClientsIterator client );
    bool stateRecieveStatus( const QString& cmd, ClientsIterator client );
    bool checkProtocolVersion( int version );
    bool isUserConnected(const QString& cmd);
    CheckUserStatus checkUserLogin(
        const QString& login,
        const QString& password
    );
    bool registerUserLogin( const QString& login, const QString& password );
    void recordSessionStatistic(
        const QString& winner,
        const QString& looser
    );

private:
    QTcpServer* tcpServer_;
    bool spawned_;
    bool guestAllowed_;
    bool registrationAllowed_;
    QHostAddress address_;
    quint16 port_;
    QString authFile_;
    QString statFile_;
    Clients clients_;
    Statistics stats_;
};
