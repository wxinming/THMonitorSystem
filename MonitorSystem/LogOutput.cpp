#include "LogOutput.h"

QString LogOutput::getDateTime()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
}

LogOutput::LogOutput(QObject *parent) : QObject(parent)
{

}

LogOutput::~LogOutput()
{
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

void LogOutput::info(QString info)
{
    qDebug() <<"info " << getDateTime() << info << endl;
}

void LogOutput::warning(QString warning)
{
    qDebug() <<"warning " << getDateTime() << warning << endl;
}

void LogOutput::error(QString error)
{
    qDebug() <<"error " << getDateTime() << error << endl;
}

