#pragma once
#include <QVector>
#include <QString>
#include <QDebug>

enum Cell
{
    CL_CLEAR=0,
    CL_SHIP,
    CL_DOT,
    CL_HALF
};

class Field
{
public:
    void initField(const QString& initData);
    Cell getCell(int x, int y);
    void setCell(int x, int y, Cell cell);
    static bool isFieldCorrect(const QString& fieldstr);
private:
    QVector<Cell> field;
};
