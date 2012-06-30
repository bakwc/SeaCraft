#include "Field.h"

void Field::initField( const QString& initData )
{
    for(
        QString::const_iterator i = initData.constBegin();
        i < initData.constEnd();
        i++
    )
        field.push_back( Cell(QString(*i).toInt()) );

    killedShips = 0;
}

Cell Field::getCell( int x, int y ) const
{
    int n = y * 10 + x;

    if( n >= 0 && n < field.size() )
        return field[n];

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
}

bool Field::isFieldCorrect( const QString& fieldstr )
{
    // TODO: 100 - width*height
    if( fieldstr.size() != 100 )
        return false;

    return true;
}

bool Field::checkField()
{
    return (
        shipNum(1) == 4 &&
        shipNum(2) == 3 &&
        shipNum(3) == 2 &&
        shipNum(4) == 1
    );
}

int Field::shipNum( int size ) const
{
    int shipNumber = 0;

    for( int i = 0; i < 10; i++ )
        for( int j = 0; j < 10; j++ )
            if( isShip(size, i, j) )
                shipNumber++;

    return shipNumber;
}

bool Field::isShip( int size, int x, int y ) const
{
    // left field !clear
    if( x > 0 && getCell(x - 1, y) != CL_CLEAR )
        return false;

    // up field !clear
    if( y > 0 && getCell(x, y - 1) != CL_CLEAR )
        return false;

    // no ship here
    if( getCell(x, y) == CL_CLEAR )
        return false;

    int tmp = x;
    int num = 0;

    // checking in right direction
    while( getCell(tmp, y) != CL_CLEAR && tmp < 10 )
    {
        tmp++;
        num++;
    }

    if( num == size )
    {
        if( getCell(x, y + 1) != CL_CLEAR )
            return false;

        return true;
    }

    tmp = y;
    num = 0;

    // checking in down direction
    while( getCell(x, tmp) != CL_CLEAR && tmp < 10 )
    {
        tmp++;
        num++;
    }

    if( num == size )
    {
        if( getCell(x + 1, y) != CL_CLEAR )
            return false;

        return true;
    }

    return false;
}

int Field::getKilledShips()
{
    return killedShips;
}

void Field::addKilledShip()
{
    killedShips++;
}
