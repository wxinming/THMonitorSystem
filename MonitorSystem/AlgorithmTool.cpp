#include "AlgorithmTool.h"

AlgorithmTool *AlgorithmTool::m_self = nullptr;
AlgorithmTool::AlgorithmTool(QObject *parent) : QObject(parent)
{

}

AlgorithmTool::~AlgorithmTool()
{
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

AlgorithmTool *AlgorithmTool::createAlgorithmTool()
{
    if (!m_self)
    {
        m_self = new AlgorithmTool;
    }
    return  m_self;
}

void AlgorithmTool::deleteAlgorithmTool()
{
    if (m_self)
    {
        delete m_self;
        m_self = nullptr;
    }
}

qreal AlgorithmTool::calcTimeForLines(QString dateTime)
{
    qreal result;
    do{
        QStringList dateTimeList = dateTime.split(" ",QString::SkipEmptyParts);
        if (!dateTimeList.size())
        {
            result = 0.0;
            break;
        }

        QString time = dateTimeList.at(1);
        QStringList timeList = time.split(":",QString::SkipEmptyParts);
        if (!timeList.size())
        {
            result = 0.0;
            break;
        }
        int hour = timeList.at(0).toInt();
        int minute = timeList.at(1).toInt();
        result = static_cast<double>(hour) + (static_cast<double>(minute) / 100 / 0.6);
    }while(false);
    return result;
}
