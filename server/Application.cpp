#include <QDebug>
#include <QFile>
#include <QStringList>
#include <iostream>
#include "Application.h"

Application::Application( int& argc, char* argv[] ):
    QCoreApplication( argc, argv ),
    server_( new Server(this) ),
    forceQuit_( false )
{
}

Application::~Application()
{
}

bool Application::init()
{
    parseCmdLine();
    if( forceQuit_ || !spawnServer() )
        return false;

    return true;
}

void Application::parseCmdLine()
{
    const QStringList& args = arguments();
    for( int i = 0; i < args.count(); i++ )
    {
        if( parseOptions(args.at(i)) )
            continue;

        if( i < args.count() - 1 )
            parseOptionsWithParam( args.at(i), args.at(i + 1) );
    }
}

bool Application::parseOptions( const QString& arg )
{
    if( isOption(arg, "-h", "--help") )
    {
        std::cout
            << "Usage:\n"
            << "    -s, --statfile [FILE]  -- use FILE for statistic "
            << "(default '" << qPrintable( DEFAULT_STAT_FILE ) << "')\n"
            << "    -u, --authfile [FILE]  -- use FILE for authorized users "
            << "(default '" << qPrintable( DEFAULT_AUTH_FILE ) << "')\n"
            << "    -a, --address  [ADDR]  -- server address to spawn to "
            << "(default "
            << qPrintable( DEFAULT_SERVER_ADDRESS.toString() ) << ")\n"
            << "    -p, --port [PORT]      -- and it's port "
            << "(default " << DEFAULT_SERVER_PORT << ")\n"
            << "    -g, --allowguest       -- allow guest accounts\n"
            << "    -r, --disablereg       -- disallow registration\n"
        ;
        forceQuit_ = true;
        return true;
    }

    if( isOption(arg, "-g", "--allowguest") )
    {
        server_->setGuestAllowed( true );
        return true;
    }

    if( isOption(arg, "-r", "--disablereg") )
    {
        server_->setRegistrationAllowed( false );
        return true;
    }

    return false;
}

bool Application::parseOptionsWithParam(
    const QString& arg,
    const QString& param
)
{
    if( isOption(arg, "-s", "--statfile") )
    {
        server_->setStatFile( param );
        return true;
    }

    if( isOption(arg, "-u", "--authfile") )
    {
        server_->setAuthFile( param );
        return true;
    }

    if( isOption(arg, "-a", "--address") )
    {
        server_->setAddress( param );
        return true;
    }

    if( isOption(arg, "-p", "--port") )
    {
        server_->setPort( param.toUInt() );
        return true;
    }

    return false;
}

bool Application::isOption( const QString& arg, const QString& option )
{
    return arg.compare( option ) == 0;
}

bool Application::isOption(
    const QString& arg,
    const QString& option1,
    const QString& option2
)
{
    return isOption( arg, option1 ) || isOption( arg, option2 );
}

bool Application::spawnServer()
{
    return server_->spawn();
}
