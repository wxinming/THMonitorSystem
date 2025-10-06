#include "StatisticalChart.h"
#include "ui_StatisticalChart.h"

StatisticalChart::StatisticalChart(int monitor, QString start, QString end,QWidget *parent):
    QWidget(parent),
    ui(new Ui::StatisticalChart)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    /*设置界面背景*/
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background,QColor(42,44,53));
    this->setPalette(palette);

    QString title = QString("监测点%1 %2 ~ %3 统计图").arg(monitor).arg(start).arg(end);
    m_databae = DatabaseTool::createDatabase();
    initChart(title);
    addNodeThread *nodeThread = new addNodeThread(monitor,start,end,m_tLine,m_hLine);
    nodeThread->start();
}

StatisticalChart::~StatisticalChart()
{
    delete ui;
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

void StatisticalChart::initChart(QString title)
{
    QChart *chart = new QChart;
    chart->setTheme(QChart::ChartThemeDark);
    chart->setTitle(title);
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
    m_tLine = new QLineSeries(ui->chartView);
    m_hLine = new QLineSeries(ui->chartView);

    m_tLine->setName("温度");
    m_hLine->setName("湿度");

    QPen pen;
    pen.setStyle(Qt::DotLine);
    pen.setWidth(1);
    pen.setColor(QColor(255,0,0));

    m_tLine->setPen(pen);
    pen.setColor(QColor(0,255,0));
    m_hLine->setPen(pen);

    chart->addSeries(m_tLine);
    chart->addSeries(m_hLine);

    QValueAxis *xAxis = new QValueAxis;
    xAxis->setRange(0,24);//设置坐标轴范围
    xAxis->setLabelFormat("%.2f");//标签格式
    xAxis->setTickCount(4);//主分隔个数
    xAxis->setMinorTickCount(7);


    QValueAxis *yAxis = new QValueAxis;
    yAxis->setRange(-10,100);
    yAxis->setLabelFormat("%.1f");

    chart->setAxisX(xAxis,m_tLine);
    chart->setAxisX(xAxis,m_hLine);
    chart->setAxisY(yAxis,m_tLine);
    chart->setAxisY(yAxis,m_hLine);
}

bool sortGenerateData(const generateData_t &g1,const generateData_t &g2)
{
    return g1.xAxis < g2.xAxis;
}


void addNodeThread::run()
{
    sortDatabaseData();
    appendNode();
}

addNodeThread::addNodeThread(int monitor,QString start,QString end,QLineSeries *tLine,
                             QLineSeries *hLine, QObject *parent):QThread(parent)
{
    m_tLine = tLine;
    m_hLine = hLine;
    setSqlCondition(monitor,start,end);
}

void addNodeThread::setSqlCondition(int monitor, QString start, QString end)
{
    m_sqlStatement = QString("select * from record where monitor=%1 and dateTime between \"%2\" and \"%3\";")
            .arg(monitor).arg(start).arg(end);
}

void addNodeThread::sortDatabaseData()
{
    do{
        QSqlQuery query(DatabaseTool::createDatabase()->getSqlDatabase());
        if (!query.exec(m_sqlStatement))
        {
            break;
        }
        generateData_t data;
        while (query.next()){
            data.xAxis = AlgorithmTool::createAlgorithmTool()->
                    calcTimeForLines(query.value(2).toDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
            data.temperature = query.value(3).toDouble();
            data.humidity = query.value(4).toDouble();
            m_generateData.push_back(data);
        }
        sort(m_generateData.begin(),m_generateData.end(),sortGenerateData);
    }while(false);
}

void addNodeThread::appendNode()
{
    QList<QPointF> tPoints;
    QList<QPointF> hPoints;
    for(auto it = m_generateData.begin();it != m_generateData.end();++it)
    {
        tPoints.push_back(QPointF(it->xAxis,it->temperature));
        hPoints.push_back(QPointF(it->xAxis,it->humidity));
    }
    m_tLine->replace(tPoints);
    m_hLine->replace(hPoints);
}

addNodeThread::~addNodeThread()
{

}
