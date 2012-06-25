#include "Field.h"

Field::Field()
{
    clear();
}

Field::~Field()
{
}

Cell Field::getCell(int x, int y)
{
    int n=y*10+x;
    if (n>=0 && n<field.size())
        return field[n];
    //qDebug() << "ERROR: no such cell";
    return CL_CLEAR;
}

void Field::setCell(int x, int y, Cell cell)
{
    int n=y*10+x;
    if (n>=0 && n<field.size())
    {
        field[n]=cell;
        return;
    }
    qDebug() << "ERROR: no such cell";
}

QString Field::getField()
{
    QString result;
    for (QVector<Cell>::iterator i=field.begin();i!=field.end();i++)
        if (*i==CL_CLEAR) result+="0";
        else result+="1";
    return result;
}

void Field::clear()
{
    field.fill(CL_CLEAR,100);
}
