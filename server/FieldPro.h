#pragma once

#include <QVector>

const quint16 DEFAULT_SHIP_SIZE = 4;

class FieldPro
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
    FieldPro( int shipSize = DEFAULT_SHIP_SIZE );
    void initField( const QString& stringField );
    Cell getCell( int x, int y ) const;
    void setCell( int x, int y, Cell cell );
    bool checkField();

    void showField();

private:
    int shipSize_;
    int fieldLength_;
    Cells field_;
};
