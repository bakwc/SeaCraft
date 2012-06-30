#pragma once

#include <QtNetwork/QTcpSocket>
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
        ST_MAKING_STEP
    };

    void send( const QString& cmd );

    QTcpSocket* socket;
    ClientStatus status;
    ClientIterator playingWith;
    Field field;
    QString login;
};

typedef QMap<int, Client> Clients;
typedef Clients::iterator ClientsIterator;
