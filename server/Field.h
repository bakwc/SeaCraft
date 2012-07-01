#pragma once

#include <QVector>
#include <QStack>
#include <QPoint>
#include "Constants.h"

class Field
{
public:
    enum Cell
    {
        CI_CLEAR = 0,
        CI_CENTER,
        CI_TOP,
        CI_BOTTOM,
        CI_VMIDDLE,
        CI_LEFT,
        CI_RIGHT,
        CI_HMIDDLE,
        CellShipsTypeCount,
        CI_DOT = 100,
        CI_DAMAGED
    };

    typedef QVector<Cell> Cells;
    typedef QVector<int> ShipsTypeList;
    typedef QStack<QPoint> Shots;

public:
    Field( int shipSize = DEFAULT_SHIP_SIZE );
    void initField( const QString& stringField );
    Cell getCell( int x, int y ) const;
    void setCell( int x, int y, Cell cell );
    bool checkField() const;
    int getShipSize() const;
    int getFieldLength() const;
    int getFieldSize() const;
    int getShipsCount() const;
    bool isAllKilled() const;
    bool makeShot( int x, int y, Shots& killShots );
    void addKilledShip( int shipSize );

    void showField() const;

private:
    void convertField();
    Cell getCellPrivate( int x, int y, const Cells& cells ) const;
    void setCellPrivate( int x, int y, Cell cell, Cells& cells ) const;

private:
    int shipSize_;
    int fieldLength_;
    Cells field_;
    ShipsTypeList killedShips_;
};
