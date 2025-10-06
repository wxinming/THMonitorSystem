#ifndef LOGOUTPUT_H
#define LOGOUTPUT_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
class LogOutput : public QObject
{
    Q_OBJECT
private:
    static QString getDateTime();
    explicit LogOutput(QObject *parent = nullptr);
    ~LogOutput();
    static LogOutput *m_self;
public:
    static void info(QString info);
    static void warning(QString warning);
    static void error(QString error);
signals:

public slots:
};

#endif // LOGOUTPUT_H
