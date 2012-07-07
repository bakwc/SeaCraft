// PlaySound_nix.h -- linux version

#pragma once

#include <QFile>
#include <alsa/asoundlib.h>
#include "PlaySound.h"

namespace PlaySoundSpace
{

typedef struct
{
    u_int32_t dwSize;
    u_int16_t wFormatTag;
    u_int16_t wChannels;
    u_int32_t dwSamplesPerSec;
    u_int32_t dwAvgBytesPerSec;
    u_int16_t wBlockAlign;
    u_int16_t wBitsPerSample;
} WAVEFORMAT;

class PlaySoundThread : public QThread
{
    Q_OBJECT
public:
    explicit PlaySoundThread( QObject* parent = 0 );
    explicit PlaySoundThread( const QString& filename, QObject* parent = 0 );
    ~PlaySoundThread();
    const QString& soundFile() const;
    void setSoundFile( const QString& filename );

public slots:
    void run();
    void quit();
    void terminate();

private:
    bool initSound( const QString& fileName );

private:
    QString soundFileName_;
    QFile sf;
    bool aboutToQuit;
    bool soundInited;

private:
    snd_pcm_t* handle;
    snd_pcm_sframes_t frames;
    snd_pcm_uframes_t chunk_size;
    snd_pcm_uframes_t buffer_size;
    size_t bits_per_sample;
    size_t bits_per_frame;
    size_t chunk_bytes;

    u_long samples;
    u_long datastart;

    WAVEFORMAT waveformat;
};

}
