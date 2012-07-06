#pragma once

#include <QtNetwork/QTcpSocket>
#include <ctime>
#include "Field.h"

class Client
{
public:
    typedef QMap<int, Client>::iterator ClientIterator;

    enum ClientStatus
    {
        ST_CONNECTED = 0,
        ST_AUTHORIZED,
        ST_READY,
        ST_WAITING_STEP,
        ST_MAKING_STEP,
        ST_DISCONNECTED
    };

    explicit Client();
    ~Client();

    void send( const QString& cmd );

    QTcpSocket* socket;
    ClientStatus status;
    ClientIterator playingWith;
    QString login;

    void setField( const QString& field, int shipSize );
    Field* field();
    void setSeen();
    size_t lastSeen();

private:
    Field* field_;
    size_t lastSeen_;
};

typedef QMap<int, Client> Clients;
typedef Clients::iterator ClientsIterator;
