#include "Utility.h"

QPoint getMyFieldCoord( const QPoint& pos )
{
    QPoint res;
    res.setX( -1 );
    res.setY( -1 );

    if(
        pos.x() < MYFIELD_X || pos.x() > (MYFIELD_X + FIELD_WIDTH) ||
        pos.y() < MYFIELD_Y || pos.y() > (MYFIELD_Y + FIELD_HEIGHT)
    )
        return res;

    res.setX( 1.0 * (pos.x() - MYFIELD_X) / (0.1 * FIELD_WIDTH) );

    qDebug() << "X: " << pos.x() - MYFIELD_X;
    qDebug() << "Y: " << pos.y() - MYFIELD_Y;

    res.setY( 1.0 * (pos.y() - MYFIELD_Y) / (0.1 * FIELD_HEIGHT) );
    return res;
}

QPoint getEnemyFieldCoord( const QPoint& pos )
{
    QPoint res;
    res.setX( -1 );
    res.setY( -1 );

    if(
        pos.x() < ENEMYFIELD_X || pos.x() > (ENEMYFIELD_X + FIELD_WIDTH) ||
        pos.y() < ENEMYFIELD_Y || pos.y() > (ENEMYFIELD_Y + FIELD_HEIGHT)
    )
        return res;

    res.setX( 1.0 * (pos.x() - ENEMYFIELD_X) / (0.1 * FIELD_WIDTH) );
    res.setY( 1.0 * (pos.y() - ENEMYFIELD_Y) / (0.1 * FIELD_HEIGHT) );
    return res;
}
