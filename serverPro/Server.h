#pragma once

#include <QObject>
#include <QHostAddress>
#include <QtNetwork/QTcpServer>
#include "constants.h"

class Server : public QObject
{
    Q_OBJECT
public:
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

private:
    QTcpServer* tcpServer_;
    bool guestAllowed_;
    QHostAddress address_;
    quint16 port_;
    QString authFile_;
    QString statFile_;
};
