#include "InterfaceLayout.h"
#include "ui_InterfaceLayout.h"

int InterfaceLayout::m_total = 0;
int InterfaceLayout::m_monitor = 0;
const QString InterfaceLayout::m_sheet =
        //正常状态样式
        "QPushButton{"
        "background-color:rgb(144,238,144);"//背景色（也可以设置图片）
        "border-style:outset;"                  //边框样式（inset/outset）
        "border-width:4px;"                     //边框宽度像素
        "border-radius:10px;"                   //边框圆角半径像素
        "border-color:rgba(255,255,255,30);"    //边框颜色
        "font:18px;"                       //字体，字体大小
        "color:rgb(139,0,0);"                //字体颜色
        "padding:6px;"                          //填衬
        "}"
        //鼠标按下样式
        "QPushButton:pressed{"
        "background-color:rgb(255,20,147);"
        "border-color:rgba(255,255,255,30);"
        "border-style:inset;"
        "color:rgba(0,0,0,100);"
        "}"
        //鼠标悬停样式
        "QPushButton:hover{"
        "background-color:rgba(100,255,100,100);"
        "border-color:rgba(255,255,255,200);"
        "color:rgb(255,255,255);"
        "}";

InterfaceLayout::InterfaceLayout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterfaceLayout),
    m_switch(false)
{
    ui->setupUi(this);
    m_total++;
    /*初始化*/
    initUi();

    initTableView();

    initChart();
    ui->verticalLayout->setMargin(1);
    ui->horizontalLayout->setMargin(1);
    ui->horizontalLayout_2->setMargin(1);
}

InterfaceLayout::~InterfaceLayout()
{
    delete ui;
    delete m_networkThread;
    delete m_model;
    m_total--;
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

/*初始化网络处理线程*/
void InterfaceLayout::initNetworkThread(const QString &ip,const ushort &port,const QString &order)
{
    m_networkThread = new NetworkThread;
    /*配置网络线程参数*/
    m_networkThread->setTableViewModel(m_model);


    m_networkThread->setChartLineSeries(static_cast<QLineSeries *>(ui->chartView->chart()->series().at(0)),
                static_cast<QLineSeries *>(ui->chartView1->chart()->series().at(0)));

    m_networkThread->setIpAndPort(ip,port);
    m_networkThread->setOrder(order);
    m_networkThread->setPlace(++m_monitor);
    m_networkThread->loadHistoryData();
    /*接收网络连接状态，从而改变Label显示变化*/
    connect(m_networkThread,&NetworkThread::changeStatusLabel,this,&InterfaceLayout::recvStatusSlot);
    /*停止networkthread重连定时器*/
    connect(this,&InterfaceLayout::stopReconnectTimerSignal,m_networkThread,&NetworkThread::stopReconnectSlot);
    /*改变tableview颜色*/
    connect(m_networkThread,&NetworkThread::changeTableViewColorSignal,
            this,&InterfaceLayout::changeTableViewColorSlot);
}

void InterfaceLayout::initUi()
{
    /*控制栏灰色背景*/
    setStatusLabelLogic(statusLabel::disconnected);
    ui->statusLabel->setFixedWidth(40);
    /*按钮初始化*/
    ui->monitoringButton->setText("监测开关");
    ui->monitoringButton->setStyleSheet(m_sheet);

    /*监控开关按键*/
    connect(ui->monitoringButton,&QPushButton::clicked,this,&InterfaceLayout::monitoringButtonSlot);

    /*控制栏标签*/
    ui->controlLabel->setText("已\n关\n闭");
    ui->controlLabel->setFixedWidth(40);

    ui->controlLabel->setStyleSheet("background-color:grey;font-size:20px;color:white;"
                                    "border:2px groove gray;border-radius:10px;padding:2px 4px");

    /*连接错误状态标签闪烁*/
    connect(&m_errorTimer,&QTimer::timeout,this,&InterfaceLayout::errorTimerSlot);
    connect(&m_errorTimer1,&QTimer::timeout,this,&InterfaceLayout::errorTimer1Slot);
}

/*设置状态标签逻辑*/
void InterfaceLayout::setStatusLabelLogic(int networkStatus,const char *statusMessage)
{
    if (networkStatus != statusLabel::status)
    {
        ui->monitoringButton->setEnabled(true);
        ui->monitoringButton->setStyleSheet(m_sheet);
    }

    if (networkStatus == statusLabel::disconnected)
    {
        /*未连接状态，灰色背景，字体颜色白色*/
        ui->statusLabel->setText("连\n接\n已\n断\n开");
        ui->statusLabel->setStyleSheet("background-color:gray;font-size:20px;color:white;"
                                       "border:2px groove gray;border-radius:10px;padding:2px 4px");
        if (m_errorTimer.isActive())
            m_errorTimer.stop();
        if (m_errorTimer1.isActive())
            m_errorTimer1.stop();

    }
    else if(networkStatus == statusLabel::connected)
    {
        /*已连接状态，绿色背景，字体颜色黑色*/
        ui->statusLabel->setText("网\n络\n正\n常");
        ui->statusLabel->setStyleSheet("background-color:green;font-size:20px;color:white;"
                                       "border:2px groove gray;border-radius:10px;padding:2px 4px");
    }
    else if(networkStatus == statusLabel::error)
    {
        /*异常连接状态，红色背景，字体颜色黑色*/
        ui->statusLabel->setText("网\n络\n异\n常");
        ui->statusLabel->setToolTip(statusMessage);
        ui->statusLabel->setStyleSheet("background-color:red;font-size:20px;color:white;"
                                       "border:2px groove gray;border-radius:10px;padding:2px 4px");
        if (!m_errorTimer.isActive())
            m_errorTimer.start(200);

    }
    else if(networkStatus == statusLabel::status)
    {
        ui->statusLabel->setText("正\n在\n连\n接");
        ui->statusLabel->setToolTip(statusMessage);
        ui->statusLabel->setStyleSheet("background-color:blue;font-size:20px;color:white;"
                                       "border:2px groove gray;border-radius:10px;padding:2px 4px");
    }
    else if (networkStatus == statusLabel::reconnect)
    {
        ui->statusLabel->setText("等\n待\n重\n连");
        ui->statusLabel->setStyleSheet("background-color:rgb(255,165,79);font-size:20px;color:white;"
                                       "border:2px groove gray;border-radius:10px;padding:2px 4px");
    }
    else if(networkStatus == statusLabel::shutdown)
    {
        /*已弃用*/
        ui->statusLabel->setText("监测点丢失");
        ui->statusLabel->setStyleSheet("background-color:yellow;font-size:20px;color:white;"
                                       "border:2px groove gray;border-radius:10px;padding:2px 4px");
    }
    ui->statusLabel->setWordWrap(true);
    ui->statusLabel->setAlignment(Qt::AlignCenter);
}

void InterfaceLayout::setControlLabelShow(bool onOff)
{
    if (onOff)
    {
        ui->controlLabel->setText("开\n启\n中");
        ui->controlLabel->setStyleSheet("background-color:green;font-size:20px;color:white;"
                                        "border:2px groove gray;border-radius:10px;padding:2px 4px");
    }
    else
    {
        ui->controlLabel->setText("已\n关\n闭");
        ui->controlLabel->setStyleSheet("background-color:grey;font-size:20px;color:white;"
                                        "border:2px groove gray;border-radius:10px;padding:2px 4px");
        emit stopReconnectTimerSignal();
        setStatusLabelLogic(statusLabel::disconnected);
    }
}

/*监控开关按钮槽函数*/
void InterfaceLayout::monitoringButtonSlot()
{
    if (!m_switch)
    {
        /*开启线程*/
        ui->monitoringButton->setEnabled(false);
        ui->monitoringButton->setStyleSheet("QPushButton{"
                                            "background-color:gray;"//背景色（也可以设置图片）
                                            "border-style:outset;"                  //边框样式（inset/outset）
                                            "border-width:4px;"                     //边框宽度像素
                                            "border-radius:10px;"                   //边框圆角半径像素
                                            "border-color:rgba(255,255,255,30);"    //边框颜色
                                            "font:18px;"                       //字体，字体大小
                                            "color:rgb(255,255,0);"                //字体颜色
                                            "padding:6px;"                          //填衬
                                            "}");
        m_networkThread->start();
        setControlLabelShow(true);
    }
    else/*ep30*/
    {
        /*退出线程*/
        if(m_networkThread->isRunning())
        {
            m_networkThread->exit();
        }
        setControlLabelShow(false);
    }
    m_switch = !m_switch;
}

/*状态错误定时器槽函数*/
/*m_errorTimer和m_errorTimer1两个定时器循环交错,实现label闪烁*/
void InterfaceLayout::errorTimerSlot()
{
    ui->statusLabel->setStyleSheet("background-color:red;font-size:20px;color:white;"
                                   "border:2px groove gray;border-radius:10px;padding:2px 4px");

    if (!m_errorTimer1.isActive())
    {
        m_errorTimer.stop();
        m_errorTimer1.start(100);
    }
#if 0
    qDebug() << __FUNCTION__ << endl;
#endif
}

/*状态错误定时器1槽函数*/
void InterfaceLayout::errorTimer1Slot()
{
    ui->statusLabel->setStyleSheet("background-color:white;font-size:20px;color:black;"
                                   "border:2px groove gray;border-radius:10px;padding:2px 4px");
    if (!m_errorTimer.isActive())
    {
        m_errorTimer.start(200);
        m_errorTimer1.stop();
    }
#if 0
    qDebug() << __FUNCTION__ << endl;
#endif
}

int InterfaceLayout::getTotal()
{
    return m_total;
}

NetworkThread *InterfaceLayout::getNetworkThread()
{
    return m_networkThread;
}

/*接收networkthread信号，改变label显示状态*/
void InterfaceLayout::recvStatusSlot(int networkStatus,const char *statusMessage)
{
    setStatusLabelLogic(networkStatus,statusMessage);
}

void InterfaceLayout::oneKeyStartSlot()
{
    if (m_switch)
        return;
    monitoringButtonSlot();
}

void InterfaceLayout::oneKeyStopSlot()
{
    if (!m_switch)
        return;
    monitoringButtonSlot();
}

void InterfaceLayout::changeTableViewColorSlot(bool alarm)
{
    if (alarm)
    {
        ui->tableView->setStyleSheet("color:rgb(255,0,0);background-color:black;gridline-color:#6c6c6c;");
    }
    else
    {
        ui->tableView->setStyleSheet("color:rgb(0,250,154);background-color:black;gridline-color:#6c6c6c;");
    }

}

/*初始化表*/
void InterfaceLayout::initTableView()
{
    /*设置表头显示标题*/
    QStringList labels = QString::fromUtf8("监测点,温度℃,湿度%").simplified().split(",");

    m_model = new QStandardItemModel();
    m_model->setHorizontalHeaderLabels(labels);
    ui->tableView->setModel(m_model);
    ui->tableView->setEnabled(false);

//    ui->tableView->horizontalHeader()->setToolTip("");

    ui->tableView->show();
    QFont font;
    font.setFamily("DigifaceWide");
    font.setPixelSize(JsonTool::createJsonTool()->getUserConfig("fontSize").toInt());
    font.setBold(true);

    /*rgb(0,250,154)MedSpringGreen*/
    ui->tableView->setStyleSheet("color:rgb(0,250,154);background-color:black;gridline-color:#6c6c6c;");
    ui->tableView->setFont(font);
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section "
                                                     "{background-color:darkcyan;"
                                                           "color: white;padding-left:"
                                                     " 4px;border: 1px solid #6c6c6c;}");
    font.setFamily("宋体");
    font.setPixelSize(30);
    ui->tableView->horizontalHeader()->setFont(font);

    ui->tableView->verticalHeader()->hide();

    /*显示网格*/
    ui->tableView->setShowGrid(true);

    /*设置网格样式，实线*/
    ui->tableView->setGridStyle(Qt::SolidLine);

    /*设置列宽不可变动*/
    ui->tableView->horizontalHeader()->setDisabled(true);

    /*设置选中时为整行*/
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    /*禁止编辑表中数据*/
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    /*自适应表内数据*/
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStandardItem* item = nullptr;

    /*监测点序号*/
    item = new QStandardItem(QString("%1").arg(m_total));
    item->setTextAlignment(Qt::AlignCenter);
    m_model->setItem(0,0,item);

    /*温度*/
    item = new QStandardItem("0.0");
    item->setTextAlignment(Qt::AlignCenter);
    m_model->setItem(0,1,item);

    /*湿度*/
    item = new QStandardItem("0.0");
    item->setTextAlignment(Qt::AlignCenter);
    m_model->setItem(0,2,item);

    return;
}

/*初始化统计图*/
void InterfaceLayout::initChart()
{
    QChart *chart = new QChart();
    QChart *chart1 = new QChart();

    //chart->setTitle("温度监控图");
    chart->setTheme(QChart::ChartThemeDark);

    //chart1->setTitle("湿度监控图");
    chart1->setTheme(QChart::ChartThemeDark);

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    ui->chartView1->setChart(chart1);
    ui->chartView1->setRenderHint(QPainter::Antialiasing);

    QLineSeries *series0 = new QLineSeries(ui->chartView);
    QLineSeries *series1 = new QLineSeries(ui->chartView1);
    series0->setName("温度(监控图)");
    series1->setName("湿度(监控图)");

    QLineSeries *temperatureLowerLimit = nullptr,*temperatureUpperLimit = nullptr,
            *humidityLowerLimit = nullptr,*humidityUpperLimit = nullptr;
    {
        temperatureLowerLimit = new QLineSeries(ui->chartView);
        temperatureUpperLimit = new QLineSeries(ui->chartView);
        temperatureLowerLimit->setName("下限" + JsonTool::createJsonTool()->getUserConfig("yAxisAlarmTll") + "℃");
        temperatureUpperLimit->setName("上限" + JsonTool::createJsonTool()->getUserConfig("yAxisAlarmTul") + "℃");

        humidityLowerLimit = new QLineSeries(ui->chartView1);
        humidityUpperLimit = new QLineSeries(ui->chartView1);
        humidityLowerLimit->setName("下限" + JsonTool::createJsonTool()->getUserConfig("yAxisAlarmHll") + "%");
        humidityUpperLimit->setName("上限" + JsonTool::createJsonTool()->getUserConfig("yAxisAlarmHul") + "%");

        QPen pen;
        pen.setStyle(Qt::DotLine);
        pen.setWidth(1);
        pen.setColor(QColor(255,0,0));
        temperatureLowerLimit->setPen(pen);
        temperatureUpperLimit->setPen(pen);
        humidityLowerLimit->setPen(pen);
        humidityUpperLimit->setPen(pen);

        temperatureLowerLimit->append(0,JsonTool::createJsonTool()->getUserConfig("yAxisAlarmTll").toDouble());
        temperatureLowerLimit->append(24,JsonTool::createJsonTool()->getUserConfig("yAxisAlarmTll").toDouble());

        temperatureUpperLimit->append(0,JsonTool::createJsonTool()->getUserConfig("yAxisAlarmTul").toDouble());
        temperatureUpperLimit->append(24,JsonTool::createJsonTool()->getUserConfig("yAxisAlarmTul").toDouble());

        humidityLowerLimit->append(0,JsonTool::createJsonTool()->getUserConfig("yAxisAlarmHll").toDouble());
        humidityLowerLimit->append(24,JsonTool::createJsonTool()->getUserConfig("yAxisAlarmHll").toDouble());

        humidityUpperLimit->append(0,JsonTool::createJsonTool()->getUserConfig("yAxisAlarmHul").toDouble());
        humidityUpperLimit->append(24,JsonTool::createJsonTool()->getUserConfig("yAxisAlarmHul").toDouble());
    }

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setColor(QColor(0,206,209));
    series0->setPen(pen);//折线序列的线条设置

    pen.setStyle(Qt::SolidLine);
    pen.setColor(Qt::green);
    pen.setWidth(2);
    series1->setPen(pen);

    chart->addSeries(series0);
    chart->addSeries(temperatureLowerLimit);
    chart->addSeries(temperatureUpperLimit);
    chart1->addSeries(series1);
    chart1->addSeries(humidityLowerLimit);
    chart1->addSeries(humidityUpperLimit);

    /*温度统计图*/
    QValueAxis *axisX = new QValueAxis(ui->chartView);
    axisX->setRange(0,24);//设置坐标轴范围
    axisX->setLabelFormat("%.2f");//标签格式
    axisX->setTickCount(4);//主分隔个数
    axisX->setMinorTickCount(7);

    int refreshTime = JsonTool::createJsonTool()->getUserConfig("refreshTime").toInt() / 1000;
    axisX->setTitleText("采集时间:" + QString::number(refreshTime)+"秒");

    QValueAxis *axisY = new QValueAxis(ui->chartView);
    axisY->setRange(JsonTool::createJsonTool()->getUserConfig("yAxisTllScale").toDouble(),
                    JsonTool::createJsonTool()->getUserConfig("yAxisTulScale").toDouble());

    axisY->setTickCount(3);
    axisY->setLabelFormat("%.1f");
    axisY->setMinorTickCount(4);
    axisY->setTickInterval(0.6);

    chart->setAxisX(axisX,series0);

    chart->setAxisX(axisX,temperatureLowerLimit);
    chart->setAxisX(axisX,temperatureUpperLimit);

    chart->setAxisY(axisY,series0);
    chart->setAxisY(axisY,temperatureLowerLimit);
    chart->setAxisY(axisY,temperatureUpperLimit);

    /*湿度统计图*/
    QValueAxis *axisX1 = new QValueAxis(ui->chartView1);
    axisX1->setRange(0,24);//设置坐标轴范围
    axisX1->setLabelFormat("%.2f");//标签格式
    axisX1->setTickCount(4);//主分隔个数
    axisX1->setMinorTickCount(7);

    axisX1->setTitleText("采集时间:" + QString::number(refreshTime)+"秒");
    QValueAxis *axisY1 = new QValueAxis(ui->chartView1);
    axisY1->setRange(JsonTool::createJsonTool()->getUserConfig("yAxisHllScale").toDouble(),
                     JsonTool::createJsonTool()->getUserConfig("yAxisHulScale").toDouble());
    axisY1->setTickCount(3);
    axisY1->setLabelFormat("%.1f");
    axisY1->setMinorTickCount(4);

    chart1->setAxisX(axisX1,series1);
    chart1->setAxisX(axisX1,humidityLowerLimit);
    chart1->setAxisX(axisX1,humidityUpperLimit);

    chart1->setAxisY(axisY1,series1);
    chart1->setAxisY(axisY1,humidityLowerLimit);
    chart1->setAxisY(axisY1,humidityUpperLimit);

    chart->setMargins(QMargins(0,0,0,0));
    chart1->setMargins(QMargins(0,0,0,0));
    return;
}
