#include "Model.h"

Model::Model()
{
    myField = new Field;
    enemyField = new Field;
    state = ST_PLACING_SHIPS;
}

Model::~Model()
{
    delete myField;
    delete enemyField;
}

Cell Model::getMyCell( int x, int y ) const
{
    return myField->getCell( x, y );
}

void Model::setMyCell( int x, int y, Cell cell )
{
    myField->setCell( x, y, cell );
}

QString Model::getMyField() const
{
    return myField->getField();
}

Cell Model::getEnemyCell( int x, int y ) const
{
    return enemyField->getCell( x, y );
}

void Model::setEnemyCell( int x, int y, Cell cell )
{
    enemyField->setCell( x, y, cell );
}

QString Model::getEnemyField() const
{
    return enemyField->getField();
}

State Model::getState() const
{
    return state;
}

void Model::setState( State st )
{
    state = st;
}

void Model::setLogin( const QString& str )
{
    login = str;
}

void Model::setPassword( const QString& str )
{
    pass = str;
}

QString Model::getLogin() const
{
    return login;
}

QString Model::getPassword() const
{
    return pass;
}

bool Model::checkMyField() const
{
    // Check field for correct ship placement
    return (
        shipNum(1) == 4 &&
        shipNum(2) == 3 &&
        shipNum(3) == 2 &&
        shipNum(4) == 1
    );
}

int Model::shipNum( int size ) const
{
    int shipNumber = 0;

    for( int i = 0; i < 10; i++ )
        for( int j = 0; j < 10; j++ )
            if( isShip(size, i, j) )
                shipNumber++;

    return shipNumber;
}

bool Model::isShip( int size, int x, int y ) const
{
    // left field !clear
    if( x > 0 && myField->getCell(x - 1, y) != CL_CLEAR )
        return false;

    // up field !clear
    if( y > 0 && myField->getCell(x, y - 1) != CL_CLEAR )
        return false;

    // no ship here
    if( myField->getCell(x, y) == CL_CLEAR )
        return false;

    int tmp = x;
    int num = 0;

    // checking in right direction
    while( myField->getCell(tmp, y) != CL_CLEAR && tmp < 10 )
    {
        tmp++;
        num++;
    }

    if( num == size )
    {
        if( myField->getCell(x, y + 1) != CL_CLEAR )
            return false;

        return true;
    }

    tmp = y;
    num = 0;

    // checking in down direction
    while( myField->getCell(x, tmp) != CL_CLEAR && tmp < 10 )
    {
        tmp++;
        num++;
    }

    if( num == size )
    {
        if( myField->getCell(x + 1, y) != CL_CLEAR )
            return false;

        return true;
    }

    return false;
}

void Model::clearEnemyField()
{
    enemyField->clear();
}

void Model::clearMyField()
{
    myField->clear();
}
