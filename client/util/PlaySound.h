#pragma once

#include <QThread>

namespace PlaySoundSpace
{
    class PlaySoundThread;
}

#ifdef Q_OS_LINUX
#include "PlaySound_nix.h"
#else
#include "PlaySound_win.h"
#endif

class PlaySound : public QObject
{
    Q_OBJECT
public:
    explicit PlaySound( QObject* parent = 0 );
    explicit PlaySound( const QString& fileName, QObject* parent = 0 );
    ~PlaySound();

public slots:
    void play();
    void play( const QString& fileName );
    void stop();

private:
    PlaySoundSpace::PlaySoundThread* soundThread_;
};

