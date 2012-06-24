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
    QTcpSocket* socket;
    ClientStatus status;
    QMap<int, Client>::iterator playingWith;
    void send( const QString& cmd );
    Field field;
};

typedef QMap<int, Client> Clients;

class MainServer: public QObject
{
    Q_OBJECT
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
    bool authorize( const QString& cmd, Clients::iterator client );
    bool setField( const QString& cmd, Clients::iterator client );
    bool makeStep( const QString& cmd, Clients::iterator client );
    bool placeShips( const QString& ships, Clients::iterator client );
    void connectTwoClients(
        Clients::iterator client1,
        Clients::iterator client2
    );

private:
    QTcpServer* server;
    QTimer* timer;
    Clients clients;
    QHostAddress address;
    quint16 port;
};

