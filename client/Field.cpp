#include "Field.h"

Field::Field(Images* images,int lft, int tp,int wdth,int hgh):
    pictures(images), left(lft), top(tp), width(wdth), height(hgh)
{
    field.fill(CL_CLEAR,100);
    image = new QImage(width,height,QImage::Format_ARGB32);
}

Field::~Field()
{
    delete image;
}

const QImage& Field::getImage() const
{
    return *image;
}

void Field::redraw()
{
    image->fill(0);
    QPainter painter(image);
    double cfx=1.0*width/10.0;
    double cfy=1.0*height/10.0;
    for (int i=0;i<10;i++)
        for (int j=0;j<10;j++)
            switch(getCell(i,j))
            {
                case CL_DOT:
                painter.drawImage(i*cfx,j*cfy,pictures->get("dot"));
                break;
                case CL_HALF:
                painter.drawImage(i*cfx,j*cfy,pictures->get("half"));
                break;
                case CL_SHIP:
                painter.drawImage(i*cfx,j*cfy,pictures->get("full"));
                break;
            default: break;
            }

}

Cell Field::getCell(int x, int y)
{
    int n=y*10+x;
    if (n>=0 && n<field.size())
        return field[n];
    qDebug() << "ERROR: no such cell";
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

int Field::getX()
{
    return left;
}

int Field::getY()
{
    return top;
}

QPoint Field::getCoord(int x, int y)
{
    QPoint res;
    res.setX(-1);
    res.setY(-1);
    if (x<left || x>(left+width) || y<top || y>(top+height)) return res;
    double cfx=1.0*width/10.0;
    double cfy=1.0*height/10.0;
    res.setX(1.0*(x-left)/cfx);
    res.setY(1.0*(y-top)/cfy);
    return res;
}

QString Field::getField()
{
    QString result;
    for (QVector<Cell>::iterator i=field.begin();i!=field.end();i++)
        if (*i==CL_CLEAR) result+="0";
        else result+="1";
    return result;
}
