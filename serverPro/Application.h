#pragma once

#include <QCoreApplication>
#include "Server.h"

class Application : public QCoreApplication
{
    Q_OBJECT
public:
    explicit Application( int& argc, char* argv[] );
    ~Application();

    void parceCmdLine();
    bool spawnServer();

private:
    bool parceOptions( const QString& arg );
    bool parceOptionsWithParam( const QString& arg, const QString& param );

    bool isOption( const QString& arg, const QString& option );
    bool isOption(
        const QString& arg,
        const QString& option1,
        const QString& option2
    );

private:
    Server* server_;
};
