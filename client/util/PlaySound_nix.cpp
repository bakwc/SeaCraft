// PlaySound_nix.cpp -- linux implementation

#include <QDebug>
#include "PlaySound_nix.h"

const quint64 COPY_BUFFER_SIZE = 1024 * 1024 * 100;

using namespace PlaySoundSpace;

PlaySoundThread::PlaySoundThread( const QString& fileName, QObject* parent ):
    QThread( parent ),
    soundFileName_( fileName ),
    sf( NULL ),
    aboutToQuit( false )
{
    initSound( fileName );
}

PlaySoundThread::PlaySoundThread( QObject* parent ):
    QThread( parent ),
    sf( NULL ),
    aboutToQuit( false ),
    soundInited( false )
{
}

PlaySoundThread::~PlaySoundThread()
{
    sf.close();
}

const QString& PlaySoundThread::soundFile() const
{
    return soundFileName_;
}

void PlaySoundThread::setSoundFile( const QString& filename )
{
    soundFileName_ = filename;
}

bool PlaySoundThread::initSound( const QString& fileName )
{
    soundInited = false;
    soundFileName_ = fileName;
    QString device = "default";
    sf.setFileName( soundFileName_ );

    if( !sf.open(QFile::ReadOnly) )
    {
        qDebug() << "ERROR: cant open media file:" << soundFileName_;
        return false;
    }

    QByteArray buffer = sf.read( COPY_BUFFER_SIZE );

    int pos = buffer.indexOf( "RIFF" );
    if( pos < 0 )
    {
        qDebug() << "ERROR: not found RIFF marker";
        return false;
    }

    pos = buffer.indexOf( "WAVE" );
    if( pos < 0 )
    {
        qDebug() << "ERROR: not found WAVE marker";
        return false;
    }

    pos = buffer.indexOf( "fmt" );
    if( pos < 0 )
    {
        qDebug() << "ERROR: not found fmt marker";
        return false;
    }

    pos += 4;
    memcpy(
        &waveformat,
        buffer.mid(pos, sizeof(WAVEFORMAT)),
        sizeof(WAVEFORMAT)
    );

    pos = buffer.indexOf( "data", pos );
    if( pos < 0 )
    {
        qDebug() << "ERROR: not found data marker";
        return false;
    }

    pos += 4;
    u_long databytes = buffer.mid( pos, sizeof(u_long) ).toULong();
    samples = databytes / waveformat.wBlockAlign;
    datastart = ( (u_long)(pos + 4) );

    snd_pcm_format_t format;
    switch( waveformat.wBitsPerSample )
    {
    case 8:
        format = SND_PCM_FORMAT_U8;
        break;

    case 16:
        format = SND_PCM_FORMAT_S16_LE;
        break;

    case 32:
        format = SND_PCM_FORMAT_U32_LE;
        break;

    default:
        qDebug() << "Unsupported format:" << waveformat.wBitsPerSample;
        return false;
    }

    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca( &params );

    int error = snd_pcm_open(
        &handle,
        qPrintable(device),
        SND_PCM_STREAM_PLAYBACK,
        SND_PCM_ASYNC
    );
    if( error < 0 )
    {
        qDebug() << "ERROR: cannot open audio device" << device
            << "(" << QString(snd_strerror( error )) << ")";
        return false;
    }

    if( (error = snd_pcm_nonblock(handle, 1)) < 0 )
    {
        qDebug() << snd_strerror( error );
        return false;
    }

    if( (error = snd_pcm_hw_params_any(handle, params)) < 0 )
    {
        qDebug() << snd_strerror( error );
        return false;
    }

    error = snd_pcm_hw_params_set_access(
        handle,
        params,
        SND_PCM_ACCESS_RW_INTERLEAVED
    );

    if( error < 0 )
    {
        qDebug() << snd_strerror( error );
        return false;
    }

    if( (error = snd_pcm_hw_params_set_format(handle, params, format)) < 0 )
    {
        qDebug() << snd_strerror( error );
        return false;
    }

    error = snd_pcm_hw_params_set_channels(
        handle,
        params,
        waveformat.wChannels
    );
    if( error < 0 )
    {
        qDebug() << snd_strerror( error );
        return false;
    }

    error = snd_pcm_hw_params_set_rate_near(
        handle,
        params,
        &waveformat.dwSamplesPerSec,
        0
    );
    if( error < 0 )
    {
        qDebug() << snd_strerror( error );
        return false;
    }

    if( (error = snd_pcm_hw_params(handle, params)) < 0 )
    {
        qDebug() << snd_strerror( error );
        return false;
    }

    chunk_size = 0;
    buffer_size = 0;
    snd_pcm_hw_params_get_buffer_size( params, &buffer_size );
    snd_pcm_hw_params_get_period_size( params, &chunk_size, 0 );
    bits_per_sample = snd_pcm_format_physical_width( format );
    bits_per_frame = bits_per_sample * waveformat.wChannels;
    chunk_bytes = chunk_size * bits_per_frame / 8;

    soundInited = true;
    return true;
}

void PlaySoundThread::run()
{
    if( !soundInited && !initSound(soundFileName_) )
        return;

    if( !sf.isReadable() )
        return;

    sf.seek( datastart );

    quint64 count, f;
    QByteArray bt;
    bt.resize( buffer_size );
    while( (count = sf.read(bt.data(), buffer_size)) && !aboutToQuit )
    {
        f = count * 8 / bits_per_frame;

        while( (frames = snd_pcm_writei(handle, bt, f)) < 0 )
            snd_pcm_prepare( handle );
    }

    if( aboutToQuit )
    {
        snd_pcm_drop( handle );
        return;
    }

    snd_pcm_drain( handle );
}

void PlaySoundThread::quit()
{
    aboutToQuit = true;
}

void PlaySoundThread::terminate()
{
    aboutToQuit = true;
}
