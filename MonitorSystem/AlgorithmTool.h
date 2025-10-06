#ifndef ALGORITHMTOOL_H
#define ALGORITHMTOOL_H

#include <QObject>
#include <QDebug>

class AlgorithmTool : public QObject
{
    Q_OBJECT
private:
    explicit AlgorithmTool(QObject *parent = nullptr);
    ~AlgorithmTool();
    static AlgorithmTool *m_self;
public:
    static AlgorithmTool *createAlgorithmTool();
    static void deleteAlgorithmTool();
    AlgorithmTool(const AlgorithmTool &) = delete;
    AlgorithmTool& operator=(const AlgorithmTool &) = delete;
    qreal calcTimeForLines(QString dateTime);
signals:

public slots:
};

#endif // ALGORITHMTOOL_H
