#include "Field.h"

void Field::initField( const QString& initData )
{
    for(
        QString::const_iterator i = initData.constBegin();
        i < initData.constEnd();
        i++
    )
        field.push_back( Cell(QString(*i).toInt()) );
}

Cell Field::getCell( int x, int y )
{
    int n = y * 10 + x;

    if( n >= 0 && n < field.size() )
        return field[n];

    qDebug() << "ERROR: no such cell";
    return CL_CLEAR;
}

void Field::setCell( int x, int y, Cell cell )
{
    int n = y * 10 + x;

    if( n >= 0 && n < field.size() )
    {
        field[n] = cell;
        return;
    }

    qDebug() << "ERROR: no such cell";
}

bool Field::isFieldCorrect( const QString& fieldstr )
{
    // TODO: 100 - width*height
    if( fieldstr.size() != 100 )
        return false;

    // TODO: Check for ships correctness
    return true;
}