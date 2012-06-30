#include <QDebug>
#include <QString>
#include <qiterator.h>
#include "FieldPro.h"

const quint16 MAX_SHIP_SIZE = 10;

FieldPro::FieldPro( int shipSize ):
    shipSize_( shipSize ),
    fieldLength_( 0 )
{
    if( shipSize > MAX_SHIP_SIZE )
        return;

    for( int i = 1; i <= shipSize; i++ )
        fieldLength_ += i;

    field_.fill( CI_CLEAR, getFieldSize() );
}

void FieldPro::initField( const QString& stringField )
{
    field_.clear();

    int fieldSize = getFieldSize();
    int sSize = qMin( stringField.length(), fieldSize );
    int nSize = qMax( stringField.length(), fieldSize ) - sSize;

    Cell value;
    for(
        QString::const_iterator it = stringField.constBegin();
        it < stringField.constEnd();
        it++
    )
    {
        value = ( Cell ) QString( *it ).toInt();
        if( value >= CellSize )
            value = CI_CLEAR;
        field_.push_back( value );
    }

    for( int i = 0; i < nSize; i++ )
        field_.push_back( CI_CLEAR );
}

void FieldPro::showField()
{
    QString str;
    for( int y = 0; y < fieldLength_; y++ )
    {
        str = "";
        for( int x = 0; x < fieldLength_; x++ )
        {
            str += QString( "%1" ).arg( getCell(x, y) );
        }

        qDebug() << str;
    }
}

FieldPro::Cell FieldPro::getCell( int x, int y ) const
{
    int n = y * fieldLength_ + x;

    if( n < 0 || (quint32) n > getFieldSize() )
        return CI_CLEAR;

    return field_[n];
}

void FieldPro::setCell( int x, int y, Cell cell )
{
    int n = y * fieldLength_ + x;

    if( n < 0 || (quint32) n > getFieldSize() )
        return;

    if( cell >= CellSize )
        cell = CI_CLEAR;

    field_[n] = cell;
}

bool FieldPro::checkField()
{
    Cells field( field_ );

    QVector<int> shipsCount;
    shipsCount.fill( 0, shipSize_ );

    Cell cell;
    int n;
    int k;
    bool b;
    for( int y = 0; y < fieldLength_; y++ )
        for( int x = 0; x < fieldLength_; x++ )
        {
            cell = getCell( x, y );

            if( cell == CI_CENTER )
            {
                shipsCount[0]++;
                setCell( x, y, CI_CLEAR );
                continue;
            }

            if( cell == CI_TOP || cell == CI_LEFT )
            {
                n = 1;
                k = 0;
                b = cell == CI_LEFT;

                do
                {
                    cell = getCell( x + b*(1 + k), y + !b*(1 + k) );
                    setCell( x + b*(1 + k), y + !b*(1 + k), CI_CLEAR );

                    if(
                        (cell == CI_VMIDDLE && !b) ||
                        (cell == CI_HMIDDLE && b)
                    )
                    {
                        k++;
                        continue;
                    }

                    if(
                        (cell == CI_BOTTOM && !b) ||
                        (cell == CI_RIGHT && b)
                    )
                    {
                        n++;
                        break;
                    }
                } while( cell != CI_CLEAR );

                n += k;

                if( n < 2 || n > shipSize_ )
                    return false;

                shipsCount[n - 1]++;
                setCell( x, y, CI_CLEAR );
                continue;
            }
        }

    for( int i = 1, shipSize = shipSize_; i <= shipSize_; i++, shipSize-- )
        for( int j = 0; j < i; j++ )
            if( shipsCount[shipSize - 1] != i )
                return false;

    return true;
}

int FieldPro::getShipSize() const
{
    return shipSize_;
}

int FieldPro::getFieldLength() const
{
    return fieldLength_;
}

int FieldPro::getFieldSize() const
{
    return fieldLength_ * fieldLength_;
}
