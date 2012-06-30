#pragma once
#include <QVector>
#include <QString>
#include <QDebug>

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
    void initField( const QString& initData );
    Cell getCell( int x, int y ) const;
    void setCell( int x, int y, Cell cell );
    static bool isFieldCorrect( const QString& fieldstr );
    bool checkField();
    int getKilledShips();
    void addKilledShip();
private:
    int shipNum( int size ) const;
    bool isShip( int size, int x, int y ) const;
private:
    QVector<Cell> field;
    int killedShips;
};
