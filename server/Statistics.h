#pragma once
#include <QMap>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegExp>

struct PlayerStats
{
    int roundsWon;
    int roundsLost;
};

struct sortStruct
{
    QString player;
    int roundsLost;
};

class Statistics
{
public:
    void save( const QString& fname );
    void load( const QString& fname );
    void playerWon( const QString& login );
    void playerLost( const QString& login );
private:
    void checkPlayer( const QString& login );
    // Check for player and add it if not exists
private:
    QMap<QString, PlayerStats> data;
};
