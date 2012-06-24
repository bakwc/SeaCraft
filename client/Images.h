#pragma once
#include <QMap>
#include <QImage>
#include <QString>

class Images
{
public:
    void load();
    QImage& get( const QString& imgName );

private:
    QMap<QString, QImage> images;
};
