#ifndef STATISTICALCHART_H
#define STATISTICALCHART_H

#include <QWidget>
#include <QThread>
#include <QtCharts>
#include <QCategoryAxis>
#include <QDateTime>
#include <vector>
using namespace std;

#include "DatabaseTool.h"
#include "AlgorithmTool.h"
using namespace QtCharts;

namespace Ui {
class StatisticalChart;
}

typedef struct GenerateData{
    qreal xAxis;
    double temperature;
    double humidity;
}generateData_t;

class addNodeThread : public QThread
{
private:
    QLineSeries *m_tLine;
    QLineSeries *m_hLine;
    QString m_sqlStatement;
    vector<generateData_t> m_generateData;
protected:
    void run();
public:
    addNodeThread(int monitor,QString start,QString end,QLineSeries *tLine,
                  QLineSeries *hLine, QObject *parent = nullptr);
    void setSqlCondition(int monitor,QString start,QString end);
    void sortDatabaseData();
    void appendNode();
    ~addNodeThread();
};

class StatisticalChart : public QWidget
{
    Q_OBJECT

public:
    StatisticalChart(int monitor,QString start,QString end,QWidget *parent = nullptr);
    ~StatisticalChart();
    void initChart(QString title);
private:
    Ui::StatisticalChart *ui;
    DatabaseTool *m_databae;
    QLineSeries *m_tLine;
    QLineSeries *m_hLine;
    QString m_sqlStatement;
    vector<generateData_t> m_generateData;
};

#endif // STATISTICALCHART_H
