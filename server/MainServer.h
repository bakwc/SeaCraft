#pragma once
#include <QtNetwork/QTcpServer>
#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QTimer>
#include <QThread>
#include "Field.h"

const quint16 DEFAULT_PORT = 1234;
const quint16 DEFAULT_SEARCH_INTERVAL = 3000;

class SleeperThread : public QThread
{
public:
    static void msleep( unsigned long msecs )
    {
        QThread::msleep( msecs );
    }
};

enum ClientStatus
{
    ST_CONNECTED = 0,
    ST_AUTHORIZED,
    ST_READY,
    ST_WAITING_STEP,
    ST_MAKING_STEP
};

struct Client
{
    typedef QMap<int, Client>::iterator ClientIterator;
    typedef QMap<int, Client>::const_iterator ClientConstIterator;

    QTcpSocket* socket;
    ClientStatus status;
    ClientIterator playingWith;
    void send( const QString& cmd );
    Field field;
};

typedef QMap<int, Client> Clients;

class MainServer: public QObject
{
    Q_OBJECT
public:
    typedef Clients::iterator ClientsIterator;
    typedef Clients::const_iterator ClientsConstIterator;

public:
    MainServer();
    bool spawn();
    bool spawn(
        const QHostAddress& address,
        quint16 port
    );
    void parceCmdLine( const QStringList& arguments );

private slots:
    void onNewUserConnected();
    void receivedData();
    void onTimer();

private:
    void parseData( const QString& cmd, int clientId );
    bool authorize( const QString& cmd, ClientsIterator client );
    bool setField( const QString& cmd, ClientsIterator client );
    bool makeStep( const QString& cmd, ClientsIterator client );
    bool placeShips( const QString& ships, ClientsIterator client );
    void connectTwoClients(
        ClientsIterator client1,
        ClientsIterator client2
    );

private:
    QTcpServer* server;
    QTimer* timer;
    Clients clients;
    QHostAddress address;
    quint16 port;
};

