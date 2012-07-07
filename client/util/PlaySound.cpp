#include "PlaySound.h"

PlaySound::PlaySound( QObject* parent ):
    QObject( parent ),
    soundThread_( NULL )
{
}

PlaySound::PlaySound( const QString& fileName, QObject* parent ):
    QObject( parent ),
    soundThread_( new PlaySoundSpace::PlaySoundThread(fileName, this) )
{
}

PlaySound::~PlaySound()
{
    if( soundThread_ )
    {
        soundThread_->quit();
        soundThread_->wait();
    }
}

void PlaySound::play()
{
    if( !soundThread_ )
        return;

    soundThread_->start();
}

void PlaySound::play( const QString& fileName )
{
    soundThread_ = new PlaySoundSpace::PlaySoundThread( fileName, this );
    soundThread_->start();
}

void PlaySound::stop()
{
    if( !soundThread_ )
        return;

    soundThread_->quit();
}
