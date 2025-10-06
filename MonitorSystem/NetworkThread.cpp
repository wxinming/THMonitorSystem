#include "NetworkThread.h"

/*静态变量初始化*/
int NetworkThread::m_total = 0;
JsonTool *NetworkThread::m_jsonTool = nullptr;

int NetworkThread::m_refreshTime = 0;
double NetworkThread::m_tul = 0;
double NetworkThread::m_tll = 0;
double NetworkThread::m_hul = 0;
double NetworkThread::m_hll = 0;
int NetworkThread::m_dbWriteTime = 0;
DatabaseTool *NetworkThread::m_database = nullptr;
int NetworkThread::m_ignore = 0;
QString NetworkThread::m_mailAddress = "";
MailTool *NetworkThread::m_mailTool = nullptr;
#define RECONNECT_TIME 1000 * 60
#define RECONNECT_TIMES 60

NetworkThread::NetworkThread(QObject *parent):QThread (parent)
{
    /*第一次创建,给静态变量赋值*/
    if (!m_total)
    {
        m_jsonTool = JsonTool::createJsonTool();
        m_refreshTime = m_jsonTool->getUserConfig("refreshTime").toInt();
        m_tul = m_jsonTool->getUserConfig("yAxisAlarmTul").toDouble();
        m_tll = m_jsonTool->getUserConfig("yAxisAlarmTll").toDouble();
        m_hul = m_jsonTool->getUserConfig("yAxisAlarmHul").toDouble();
        m_hll = m_jsonTool->getUserConfig("yAxisAlarmHll").toDouble();
        m_dbWriteTime = m_jsonTool->getUserConfig("dbWriteTime").toInt();
        m_database = DatabaseTool::createDatabase();
        m_ignore = m_jsonTool->getUserConfig("ignore").toInt();
        m_mailAddress = m_jsonTool->getUserConfig("email");
        m_mailTool = MailTool::createMailTool();
    }

    /*TCP*/
    m_tcpSocket = nullptr;
    m_sendTimer = nullptr;

    m_tHRange = qMakePair(false,false);
    /*统计图x轴值*/
    m_lineSeriesCount = 0;

    m_reconnectTimer = new QTimer;

    /*重连次数*/
    m_reconnectCount = RECONNECT_TIMES;

    connect(m_reconnectTimer,&QTimer::timeout,this,&NetworkThread::reconnectSlot);

    connect(this,&QThread::finished,this,&NetworkThread::threadExitSlot);
    /*子线程注册相关信号量*/
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketError");

    connect(this,&NetworkThread::playSoundSignal,AlarmSound::createAlarmSound(),&AlarmSound::playSoundSlot);
    connect(this,&NetworkThread::stopSoundSignal,AlarmSound::createAlarmSound(),&AlarmSound::stopSoundSlot);

    m_total++;
}

NetworkThread::~NetworkThread()
{
    /*如果强制关闭应用程序，让线程安全关闭*/
    if(isRunning())
    {
        this->exit();
        this->wait();
    }

    if (m_reconnectTimer)
    {
        if (m_reconnectTimer->isActive())
        {
            m_reconnectTimer->stop();
        }
        delete m_reconnectTimer;
        m_reconnectTimer = nullptr;
    }
    m_total--;
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

/*重写QThread run*/
void NetworkThread::run()
{
#ifndef NDEBUG
    qDebug() <<"thread " << m_monitor <<" running"<< endl;
#endif
    /*界面刷新计数器*/
    m_refreshTimeCounter = 0;

    /*启动线程tableview立马显示当前数据*/
    m_firstData = true;

    /*此处必须在run中new*/
    m_tcpSocket = new QTcpSocket;

    /*发送到服务器定时器*/
    m_sendTimer = new QTimer;

    /*数据库写入定时器*/
    m_dbWriteTimer = new QTimer;

    /*监控tcp是否连接成功*/
    connect(m_tcpSocket,&QTcpSocket::connected,this,&NetworkThread::connectedSlot);

    /*监控tcp是否断开连接*/
    connect(m_tcpSocket,&QTcpSocket::disconnected,this,&NetworkThread::disconnectedSlot);

    /*监控tcp连接状态*/
    connect(m_tcpSocket,&QTcpSocket::stateChanged,this,&NetworkThread::stateChangedSlot);

    /*监控tcp错误状态*/
    connect(m_tcpSocket,static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),this,
            &NetworkThread::socketErrorSlot);

    /*连接服务端*/
#ifndef NDEBUG
    qDebug() << m_ip << m_port <<endl;
#endif

    m_tcpSocket->connectToHost(m_ip,m_port);

    /*阻塞等待连接*/
    bool result = m_tcpSocket->waitForConnected();
    if (result)
    {
        /*监控socket数据流,湿度35-70*/
        connect(m_tcpSocket,&QTcpSocket::readyRead,this,&NetworkThread::readyReadSlot);

        /*连接成功启动定时器*/
        /*与服务端通讯定时器*/
        connect(m_sendTimer,&QTimer::timeout,[=](){
            if(m_tcpSocket->write(m_order.data(),m_order.length()) == -1)
            {
                qDebug() <<"发送到服务器失败"<<endl;
            }
        });
        m_sendTimer->start(1000);

        if (DatabaseTool::createDatabase()->getState())
        {
            connect(m_dbWriteTimer,&QTimer::timeout,this,&NetworkThread::dbWriteSlot);
            m_dbWriteTimer->start(m_dbWriteTime);
        }
    }
    /*此处必须执行exec否则线程将立马退出*/
    exec();

    qDebug() <<"thread "<< m_monitor <<" exit" << endl;

    /*线程退出后清理堆区*/
    cleanUpHeap();
}

void NetworkThread::loadHistoryData()
{
    do{
        if (!m_database->getState())
            return;

        QString str = QString("select * from record where monitor=%1 and dateTime between \"%2\" and \"%3\";")
                    .arg(m_monitor).arg(getDateTime("startTime")).arg(getDateTime("endTime"));

        QSqlQuery query(m_database->getSqlDatabase());
        if (!query.exec(str))
        {
            break;
        }

        QList<QPointF> tList,hList;
        while (query.next()) {
            qreal xAxis = AlgorithmTool::createAlgorithmTool()->
                    calcTimeForLines(query.value(2).toDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
            tList.push_back(QPointF(xAxis,query.value(3).toDouble()));
            hList.push_back(QPointF(xAxis,query.value(4).toDouble()));
        }
        m_tLine->replace(tList);
        m_hLine->replace(hList);
    }while(false);
    return;
}

void NetworkThread::restoreModelColor()
{
    m_model->setData(m_model->index(0,1),QBrush(Qt::green),Qt::ForegroundRole);
    m_model->setData(m_model->index(0,2),QBrush(Qt::green),Qt::ForegroundRole);
}

/*设置json指令*/
void NetworkThread::setOrder(const QString &order)
{
    m_order = QByteArray::fromHex(order.toLatin1());
}

/*设置ip和port*/
void NetworkThread::setIpAndPort(const QString &ip,const ushort &port)
{
    m_ip = ip;
    m_port = port;
}

/*改变折线图温湿度值*/
void NetworkThread::changeChartTH(double temperature, double humidity,int hour,int minute)
{
    m_lineSeriesCount = static_cast<double>(hour) + (static_cast<double>(minute) / 100 / 0.6);
    m_tLine->append(m_lineSeriesCount,temperature);
    m_hLine->append(m_lineSeriesCount,humidity);
}

/*改变表格温湿度值*/
void NetworkThread::changeTableViewTH(double temperature, double humidity)
{
    if (!m_ignore)
    {
        whetherAlarm();
    }

    m_model->setData(m_model->index(0,1),QBrush(m_tHRange.first ? Qt::red:Qt::green),Qt::ForegroundRole);

    m_model->setData(m_model->index(0,2),QBrush(m_tHRange.second ? Qt::red:Qt::green),Qt::ForegroundRole);

    if (m_tHRange.first || m_tHRange.second)
    {
        emit playSoundSignal();
        int monitor;
        if (m_monitor % 3 == 0)
        {
            monitor = m_monitor / 3;
        }
        else
        {
            monitor = m_monitor / 3 + 1;
        }
        QString mailTitle;
        if (m_tHRange.first && m_tHRange.second)
            mailTitle = QString("位置:%1 温湿度超标(%2℃,%3%)").arg(m_monitor).arg(m_temperature).arg(m_humidity);
        else if (m_tHRange.first)
            mailTitle = QString("位置:%1 温度超标(%2℃)").arg(m_monitor).arg(m_temperature);
        else if (m_tHRange.second)
            mailTitle = QString("位置:%1 湿度超标(%2%)").arg(m_monitor).arg(m_humidity);

        QString monitorTitle = m_jsonTool->getTabWidgetConfig(monitor - 1,"title");
        QString content = QString("超标位置:\n   %1,%2号探头;\n"
                                  "超标情况:\n   温湿度规格:温度(%3℃~%4℃),湿度(%5%~%6%);\n   实测:温度(%7℃),湿度(%8%);")
                .arg(monitorTitle).arg(m_monitor).arg(m_tll).arg(m_tul).arg(m_hll).arg(m_hul).arg(m_temperature).arg(m_humidity);
        for(int i = 0;i < 3;i++)
        {
            if (m_mailTool->sendMail(m_mailAddress,mailTitle,content))
            {
                break;
            }
        }
    }
    m_model->setData(m_model->index(0,1),QString::number(temperature,'f',1));
    m_model->setData(m_model->index(0,2),QString::number(humidity,'f',1));
}

/*设置表格model*/
void NetworkThread::setTableViewModel(QStandardItemModel *model)
{
    m_model = model;
}

/*设置统计图曲线*/
void NetworkThread::setChartLineSeries(QLineSeries *tLine, QLineSeries *hLine)
{
    m_tLine = tLine;
    m_hLine = hLine;
}

void NetworkThread::setPlace(int monitor, QString place)
{
    m_monitor = monitor;
    m_place = place;
}

/*线程退出清理垃圾*/
void NetworkThread::cleanUpHeap()
{
    changeTableViewTH(0.0,0.0);
    /*解除注册信号*/
    disconnect(m_tcpSocket,&QTcpSocket::connected,this,&NetworkThread::connectedSlot);

    disconnect(m_tcpSocket,&QTcpSocket::disconnected,this,&NetworkThread::disconnectedSlot);

    disconnect(m_tcpSocket,&QTcpSocket::stateChanged,this,&NetworkThread::stateChangedSlot);

    disconnect(m_tcpSocket,static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
               this,&NetworkThread::socketErrorSlot);

    if (m_sendTimer)
    {
        if (m_sendTimer->isActive())
        {
            m_sendTimer->stop();
        }
        delete m_sendTimer;
        m_sendTimer = nullptr;
    }

    if (m_dbWriteTimer)
    {
        if (m_dbWriteTimer->isActive())
        {
            m_dbWriteTimer->stop();
        }
        delete m_dbWriteTimer;
        m_dbWriteTimer = nullptr;
    }

    if (m_tcpSocket)
    {
        if (m_tcpSocket->isOpen())
        {
            m_tcpSocket->close();
        }
        delete m_tcpSocket;
        m_tcpSocket = nullptr;
    }
    restoreModelColor();
    emit stopSoundSignal();

    if (!m_reconnectCount)
    {
        changeStatusLabel(statusLabel::error,"SocketTimeoutError");
    }
    else if(m_reconnectCount && m_reconnectTimer->isActive())
    {
        changeStatusLabel(statusLabel::reconnect);
    }
}

void NetworkThread::whetherAlarm()
{
    if (m_temperature > m_tul || m_temperature < m_tll)
    {
        m_tHRange.first = true;
    }
    else
    {
        m_tHRange.first = false;
    }

    if (m_humidity > m_hul || m_humidity < m_hll)
    {
        m_tHRange.second = true;
    }
    else
    {
        m_tHRange.second = false;
    }
}

/*socket连接成功槽*/
void NetworkThread::connectedSlot()
{
#ifndef NDEBUG
    qDebug() <<"连接成功"<<endl;
#endif
    sendStatusSignal(statusLabel::connected);
    m_reconnectCount = RECONNECT_TIMES;
}

/*socket关闭连接槽*/
void NetworkThread::disconnectedSlot()
{
#ifndef NDEBUG
    qDebug() <<"断开连接"<<endl;
#endif
    sendStatusSignal(statusLabel::disconnected);
    changeTableViewTH(0.0,0.0);
}

/*socket状态改变槽*/
void NetworkThread::stateChangedSlot(QAbstractSocket::SocketState code)
{
#ifndef NDEBUG
    qDebug() <<"连接状态:"<< code << endl;
#endif
    sendStatusSignal(statusLabel::status,
                     QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey(code));
}

/*socket错误槽*/
void NetworkThread::socketErrorSlot(QAbstractSocket::SocketError code)
{
#ifndef NDEBUG
    qDebug()<<"连接错误:"<< code <<endl;
#endif

    if (!m_reconnectCount)
    {
        sendStatusSignal(statusLabel::error,
                QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey(code));
        #ifndef NDEBUG
        qDebug() << "Error" << endl;
        #endif
    }
    else
    {
        sendStatusSignal(statusLabel::reconnect);
        if (!m_reconnectTimer->isActive() && m_reconnectCount)
        {
            m_reconnectTimer->start(RECONNECT_TIME);
            m_reconnectCount--;
#ifndef NDEBUG
            qDebug() << "reconnectCount:" << m_reconnectCount << endl;
#endif
        }
    }

    this->exit();
}

/*socket有数据过来的槽*/
void NetworkThread::readyReadSlot()
{
    QTime current = QTime::currentTime();
    /*到第二天清空以前的统计*/
    if (!current.hour() && !current.minute())
    {
        m_tLine->clear();
        m_hLine->clear();
    }

    QString hexString = m_tcpSocket->readAll().toHex();
    m_temperature = static_cast<double>(hexString.mid(18,4).insert(0,"0x").toInt(nullptr,16));
    m_humidity = static_cast<double>(hexString.mid(22,4).insert(0,"0x").toInt(nullptr,16));
    m_temperature /= 10;
    m_humidity /= 10;

    /*m_refreshTime刷新一次界面*/
    if (m_refreshTimeCounter++ * 1000 >= m_refreshTime || m_firstData)
    {
        changeTableViewTH(m_temperature,m_humidity);
        changeChartTH(m_temperature,m_humidity,current.hour(),current.minute());
        m_refreshTimeCounter = 0;
        m_firstData = false;
    }
}

void NetworkThread::dbWriteSlot()
{
    insertTable_t value;
    value.temperature = m_temperature;
    value.humidity = m_humidity;
    value.state = (m_tHRange.first || m_tHRange.second) ? "异常" : "正常";
    value.monitor = m_monitor;
    if (m_database->insertData(value))
    {
#ifndef NDEBUG
        qDebug() << m_monitor << "写入数据库成功" << endl;
#endif
    }
    else
    {
#ifndef NDEBUG
        qDebug() << m_monitor << "写入数据库失败" << endl;
#endif
    }
}

void NetworkThread::reconnectSlot()
{
    if (m_reconnectTimer->isActive())
        m_reconnectTimer->stop();
    start();
}

void NetworkThread::stopReconnectSlot()
{
#ifndef NDEBUG
    qDebug() <<"ID:" << m_monitor << __FUNCTION__ << endl;
#endif
    if (m_reconnectTimer->isActive())
    {
        #ifndef NDEBUG
        qDebug() << "Stop Reconnect Timer" << endl;
        #endif
        m_reconnectTimer->stop();
    }
}

void NetworkThread::threadExitSlot()
{
    #ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
    #endif
}

/*获取ip*/
const QString &NetworkThread::getIp() const
{
    return m_ip;
}

/*获取端口*/
const ushort &NetworkThread::getPort() const
{
    return m_port;
}

/*获取线程对象数量*/
const int &NetworkThread::getThreadTotal() const
{
    return m_total;
}

/*获取系统时间*/
const QString NetworkThread::getCurrentTime()
{
    QString time = QTime::currentTime().toString(Qt::ISODate);
    return time;
}

QString NetworkThread::getDateTime(QString name)
{
    QTime time;
    if (name == "startTime")
    {
        time.setHMS(0,0,0);
    }
    else if(name == "endTime")
    {
        time.setHMS(23,59,59);
    }
    else
    {
        return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    }
    QDateTime dateTime;
    dateTime.setDate(QDate::currentDate());
    dateTime.setTime(time);
    return dateTime.toString("yyyy-MM-dd hh:mm:ss:zzz");
}
