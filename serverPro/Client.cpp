#include "Client.h"

Client::Client():
    field_( NULL )
{
}

Client::~Client()
{
    if( field_ )
        delete field_;
}

void Client::send( const QString& cmd )
{
    QTextStream clientStream( socket );
    clientStream << cmd;
    clientStream << flush;
}

void Client::setField( const QString& field, int shipSize )
{
    field_ = new Field( shipSize );
    field_->initField( field );
}

const Field* Client::field() const
{
    return field_;
}
