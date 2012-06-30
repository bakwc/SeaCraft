#include <QDebug>
#include <QFile>
#include <QStringList>
#include "Application.h"

Application::Application( int& argc, char* argv[] ):
    QCoreApplication( argc, argv )
{
    server_ = new Server( this );
}

Application::~Application()
{
}

void Application::parceCmdLine()
{
    const QStringList& args = arguments();
    for( int i = 0; i < args.count(); i++ )
    {
        if( parceOptions(args.at(i)) )
            continue;

        if( i < args.count() - 1 )
            parceOptionsWithParam( args.at(i), args.at(i + 1) );
    }
}

bool Application::parceOptions( const QString& arg )
{
    if( isOption(arg, "-g", "--allowguests") )
    {
        qDebug() << "Server accepts guests";
        server_->setGuestAllowed( true );
        return true;
    }

    return false;
}

bool Application::parceOptionsWithParam(
    const QString& arg,
    const QString& param
)
{
    if( isOption(arg, "-s", "--statfile") )
    {
        qDebug() << "Using statistic file:" << param;
        server_->setStatFile( param );
        return true;
    }

    if( isOption(arg, "-u", "--authfile") )
    {
        qDebug() << "Using authorization file:" << param;
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
