#include <QCoreApplication>
#include <QtNetwork/QTcpServer>
#include <QObject>
#include <QDebug>
#include "MainServer.h"

int main( int argc, char* argv[] )
{
    QCoreApplication a( argc, argv );
    MainServer mserver;
    return a.exec();
}
