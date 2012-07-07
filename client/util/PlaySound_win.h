// PlaySound_win.h -- windows version

#pragma once

#include <QSound>
#include <QTemporaryFile>
#include "PlaySound.h"

namespace PlaySoundSpace
{

class PlaySoundThread : public QThread
{
    Q_OBJECT
public:
    explicit PlaySoundThread( QObject* parent = 0 );
    explicit PlaySoundThread( const QString& filename, QObject* parent = 0 );
    const QString& soundFile() const;
    void setSoundFile( const QString& filename );

public slots:
    void run();
    void quit();

private:
    bool initSound( const QString& fileName );
    bool copyFromResource( const QString& fileName );

private:
    QString soundFileName_;
    QTemporaryFile* tmpFile_;
    QSound* sound_;
};

}
