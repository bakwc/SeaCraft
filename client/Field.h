#pragma once
#include <QVector>
#include <QImage>
#include <QDebug>
#include <QPainter>
#include <QPoint>
#include <QString>
#include "Images.h"

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
    Field(Images* images,int lft, int tp,int wdth,int hght);
    ~Field();
    const QImage& getImage() const;
    void redraw();
    Cell getCell(int x, int y);
    void setCell(int x, int y, Cell cell);
    QPoint getCoord(int x, int y);
    QString getField();
    int getX();
    int getY();
private:
    QImage *image;
    Images *pictures;
    QVector<Cell> field;
    int left,top,width,height;
};
