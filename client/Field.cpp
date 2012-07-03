#include "Field.h"

Field::Field()
{
    clear();
}

Field::~Field()
{
}

Cell Field::getCell( int x, int y )
{
    int n = y * 10 + x;

    if( x < 0 || y < 0 || x >= 10 || y >= 10 )
        return CL_CLEAR;

    if( n >= 0 && n < field.size() )
        return field[n];

    //qDebug() << "ERROR: no such cell";
    return CL_CLEAR;
}

void Field::setCell( int x, int y, Cell cell )
{
    int n = y * 10 + x;

    if(
        x >= 0 && y >= 0 && x < 10 && y < 10 &&
        n >= 0 && n < field.size()
    )
    {
        field[n] = cell;
        return;
    }

    //qDebug() << "ERROR: no such cell (" << x << "x" << y << ")";
}

QString Field::getField()
{
    QString result;

    for( QVector<Cell>::iterator i = field.begin(); i != field.end(); i++ )
        if( *i == CL_CLEAR )
            result += "0";
        else
            result += "1";

    return result;
}

void Field::clear()
{
    field.fill( CL_CLEAR, 100 );
}
