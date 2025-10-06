#ifndef ALARMSOUND_H
#define ALARMSOUND_H

#include <QObject>
#include <QSoundEffect>
#include <QDebug>
class AlarmSound : public QObject
{
    Q_OBJECT
private:
    explicit AlarmSound(QObject *parent = nullptr);
    ~AlarmSound();
    static AlarmSound *m_self;
    QSoundEffect m_sound;
protected:
    void initAlarmSound();
    void playAlarmSound();
    void muteAlarmSound();
    void stopAlarmSound();
    void unmuteAlarmSound();
public:
    static AlarmSound *createAlarmSound();
    static void deleteAlarmSound();
    AlarmSound(const AlarmSound &) = delete;
    AlarmSound& operator=(const AlarmSound &) = delete;
signals:

public slots:
    void playSoundSlot();
    void muteSoundSlot();
    void unmuteSoundSlot();
    void stopSoundSlot();
};

#endif // ALARMSOUND_H
