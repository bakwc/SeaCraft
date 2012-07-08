#include "Images.h"

Images pictures = Images();

Images::Images():
    isLoaded_( false )
{
}

void Images::load()
{
    if( isLoaded_ )
        return;

    images.insert( "field", QImage(":/background.png") );
    images.insert( "dot", QImage(":/dot.png") );
    images.insert( "full", QImage(":/full.png") );
    images.insert( "half", QImage(":/half.png") );
    images.insert( "redhalf", QImage(":/redhalf.png") );
    images.insert( "redfull", QImage(":/redfull.png") );
    images.insert( "about", QImage(":/about.png") );
    isLoaded_ = true;
}

QImage& Images::get( const QString& imgName )
{
    QMap<QString, QImage>::iterator i = images.find( imgName );

    if( i == images.end() )
        throw 1;

    return i.value();
}

bool Images::isLoaded()
{
    return isLoaded_;
}
