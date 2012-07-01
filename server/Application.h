#pragma once

#include <QCoreApplication>
#include "Server.h"

class Application : public QCoreApplication
{
    Q_OBJECT
public:
    explicit Application( int& argc, char* argv[] );
    ~Application();
    bool init();

private:
    void parseCmdLine();
    bool parseOptions( const QString& arg );
    bool parseOptionsWithParam( const QString& arg, const QString& param );
    bool spawnServer();

    bool isOption( const QString& arg, const QString& option );
    bool isOption(
        const QString& arg,
        const QString& option1,
        const QString& option2
    );

private:
    Server* server_;
    bool forceQuit_;
};
