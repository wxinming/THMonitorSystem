#include "AlarmSound.h"

AlarmSound *AlarmSound::m_self = nullptr;

AlarmSound::AlarmSound(QObject *parent) : QObject(parent)
{

}

AlarmSound::~AlarmSound()
{
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

void AlarmSound::initAlarmSound()
{
#define LOAD_FILE 0
#if LOAD_FILE
    QUrl url;
    url.setPath("file:///./wav/THAlarmSound.wav");
    if (url.isValid())
    {
        m_sound.setSource(url);

#ifndef NDEBUG
        qDebug() << "isEmpty" << endl;
#endif
    }
    else
    {
#ifndef NDEBUG
       qDebug() << "noEmpty" << endl;
#endif
#endif
       m_sound.setSource(QUrl::fromLocalFile(":/wav/resources/wav/THAlarmSound.wav"));
#if LOAD_FILE
    }
#endif
    m_sound.setLoopCount(1);
    m_sound.setVolume(1);
    return;
}

void AlarmSound::playAlarmSound()
{
    if (!m_sound.isLoaded())
        return;
    if (!m_sound.isPlaying())
    {
        m_sound.play();
    }
}

void AlarmSound::muteAlarmSound()
{
    if (!m_sound.isLoaded())
        return;
    m_sound.setVolume(0);
}

void AlarmSound::stopAlarmSound()
{
    if (!m_sound.isLoaded())
        return;
    if (m_sound.isPlaying())
    {
        m_sound.stop();
    }
}

void AlarmSound::unmuteAlarmSound()
{
    if (!m_sound.isLoaded())
        return;
    m_sound.setVolume(1);
}

AlarmSound *AlarmSound::createAlarmSound()
{
    if (!m_self)
    {
        m_self = new AlarmSound;
        m_self->initAlarmSound();
    }
    return m_self;
}

void AlarmSound::deleteAlarmSound()
{
    if (m_self)
    {
        delete m_self;
        m_self = nullptr;
    }
}

void AlarmSound::playSoundSlot()
{
    playAlarmSound();
}

void AlarmSound::muteSoundSlot()
{
    muteAlarmSound();
}

void AlarmSound::unmuteSoundSlot()
{
    unmuteAlarmSound();
}

void AlarmSound::stopSoundSlot()
{
    stopAlarmSound();
}
