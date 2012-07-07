#include "Statistics.h"

void Statistics::playerWon( const QString& login )
{
    qDebug() << "Stats: won!";
    checkPlayer( login );
    data.find( login )->roundsWon++;
}

void Statistics::playerLost( const QString& login )
{
    qDebug() << "Stats: lost!";
    checkPlayer( login );
    data.find( login )->roundsLost++;
}

void Statistics::checkPlayer( const QString& login )
{
    QMap<QString, PlayerStats>::iterator player = data.find( login );

    if( player == data.end() )
    {
        PlayerStats stats;
        stats.roundsWon = 0;
        stats.roundsLost = 0;
        data.insert( login, stats );
    }
}

void Statistics::save( const QString& fname )
{
    if (data.empty()) return;

    QMultiMap<quint32,sortStruct> sortedStat;

    qDebug() << "Stats: saving";
    qDebug() << "Size of data:" << data.size();
    QFile file( fname );
    file.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream out( &file );

    for(
        QMap<QString, PlayerStats>::iterator i = data.begin();
        i != data.end();
        i++
    )
    {
        sortStruct tmp;
        tmp.player = i.key();
        tmp.roundsLost = i->roundsLost;
        sortedStat.insert(i->roundsWon,tmp);
        /*
        out << QString( "%1:%2:%3:\n" )
            .arg( i.key() )
            .arg( i->roundsWon )
            .arg( i->roundsLost );
        */
    }

    for(QMap<quint32,sortStruct>::iterator
        i=sortedStat.end()-1;i!=sortedStat.begin();i--)
    {
        out << QString( "%1:%2:%3:\n" )
            .arg( i->player )
            .arg( i.key() )
            .arg( i->roundsLost );
    }

    out << QString( "%1:%2:%3:\n" )
        .arg( sortedStat.begin()->player )
        .arg( sortedStat.begin().key() )
        .arg( sortedStat.begin()->roundsLost );

    out << flush;
    file.close();
}

void Statistics::load( const QString& fname )
{
    if( !QFile::exists(fname) )
    {
        qDebug() << "File with statistic does not exists!";
        return;
    }

    QFile af( fname );

    if( !af.open(QFile::ReadOnly) )
        return;

    QByteArray line;
    QRegExp rx( "(\\w+):(\\d+):(\\d+)" );

    while( !af.atEnd() )
    {
        line = af.readLine();

        if( rx.indexIn(line) == -1 )
            continue;

        QString login = rx.cap( 1 );
        PlayerStats stats;
        stats.roundsWon = rx.cap( 2 ).toInt();
        stats.roundsLost = rx.cap( 3 ).toInt();
        data.insert( login, stats );
    }
}
