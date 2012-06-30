#pragma once
#include <QVector>
#include <QDebug>
#include <QString>

enum Cell
{
    CL_CLEAR = 0,
    CL_SHIP,
    CL_DOT,
    CL_HALF
};

class Field
{
public:
    Field();
    ~Field();
    Cell getCell( int x, int y );
    void setCell( int x, int y, Cell cell );
    QString getField();
    void clear();

private:
    QVector<Cell> field;
};
