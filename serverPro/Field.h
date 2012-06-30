#pragma once

#include <QVector>
#include "constants.h"

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
        CI_DOT,
        CellSize
    };

    typedef QVector<Cell> Cells;

public:
    Field( int shipSize = DEFAULT_SHIP_SIZE );
    void initField( const QString& stringField );
    Cell getCell( int x, int y ) const;
    void setCell( int x, int y, Cell cell );
    bool checkField() const;
    int getShipSize() const;
    int getFieldLength() const;
    int getFieldSize() const;

    void showField() const;

private:
    Cell getCellPrivate( int x, int y, const Cells& cells ) const;
    void setCellPrivate( int x, int y, Cell cell, Cells& cells ) const;

private:
    int shipSize_;
    int fieldLength_;
    Cells field_;
};
