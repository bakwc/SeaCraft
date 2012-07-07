// PlaySound_win.cpp -- windows implementation

#include <QDebug>
#include <QFile>
#include "PlaySound_win.h"

const quint64 COPY_BUFFER_SIZE = 1024 * 1024 * 100;

using namespace PlaySoundSpace;

PlaySoundThread::PlaySoundThread( const QString& fileName, QObject* parent ):
    QThread( parent ),
    soundFileName_( fileName ),
    tmpFile_( NULL ),
    sound_( NULL )
{
    initSound( fileName );
}

PlaySoundThread::PlaySoundThread( QObject* parent ):
    QThread( parent ),
    tmpFile_( NULL ),
    sound_( NULL )
{
}

bool PlaySoundThread::initSound( const QString& fileName )
{
    soundFileName_ = fileName;

    if( fileName.at(0) == ':' )
    {
        tmpFile_ = new QTemporaryFile( this );

        if( !copyFromResource(fileName) )
        {
            qDebug( "Failed copy from resource" );
            return false;
        }

        soundFileName_ = tmpFile_->fileName();
    }

    sound_ = new QSound( soundFileName_ );
    return true;
}

void PlaySoundThread::run()
{
    if( !initSound(soundFileName_) )
        return;

    if( !sound_ )
        return;

    qDebug( "Playing %s...", qPrintable(sound_->fileName()) );
    sound_->play();
}

void PlaySoundThread::quit()
{
    if( !sound_ )
        return;

    sound_->stop();
}

const QString& PlaySoundThread::soundFile() const
{
    return soundFileName_;
}

void PlaySoundThread::setSoundFile( const QString& filename )
{
    soundFileName_ = filename;
}

bool PlaySoundThread::copyFromResource( const QString& fileName )
{
    if( !tmpFile_->open() )
    {
        qWarning(
            "WARN: PlaySoundThread::copyFromResource():"
            " Cannot create temporary file."
        );
        return false;
    }

    QFile resFile( fileName );
    if( !resFile.open(QFile::ReadOnly) )
    {
        qWarning(
            "WARN: PlaySoundThread::copyFromResource():"
            " Cannot open resource file."
        );
        return false;
    }

    QByteArray buffer( COPY_BUFFER_SIZE, '\0' );
    quint64 readSize = 0;

    while( (readSize = resFile.read(buffer.data(), COPY_BUFFER_SIZE)) > 0 )
        tmpFile_->write( buffer.data(), readSize );

    tmpFile_->flush();
    resFile.close();
    return true;
}
