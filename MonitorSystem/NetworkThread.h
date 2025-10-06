#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

/*网络处理线程类*/
#include <QThread>
#include <QTcpSocket>
#include <QTimer>
#include <QMetaEnum>
#include <QMetaType>
#include <QtCharts>
#include <QDateTime>
#include <QPair>

#include "AlgorithmTool.h"
#include "AlarmSound.h"
#include "DatabaseTool.h"
#include "JsonTool.h"
#include "MailTool.h"

class NetworkThread : public QThread
{
    Q_OBJECT
private:
    /*写入数据库定时器*/
    QTimer *m_dbWriteTimer;

    /*重新连接定时器*/
    QTimer *m_reconnectTimer;

    /*重连次数*/
    int m_reconnectCount;

    /*刷新时间计数器*/
    int m_refreshTimeCounter;

    /*第一次开启,立马显示数据*/
    bool m_firstData;

    /*T and H是否超出范围*/
    QPair<bool,bool> m_tHRange;

    /*json*/
    static JsonTool *m_jsonTool;
    static int m_refreshTime;
    static double m_tul;
    static double m_tll;
    static double m_hul;
    static double m_hll;
    static int m_dbWriteTime;
    static int m_ignore;
    static QString m_mailAddress;
    static MailTool* m_mailTool;
    /*database*/
    static DatabaseTool *m_database;

    /*t and h*/
    double m_temperature;
    double m_humidity;

    /*TCP相关变量*/
    QTcpSocket *m_tcpSocket;
    QTimer *m_sendTimer;
    QString m_ip;
    ushort m_port;

    /*监测点*/
    int m_monitor;
    /*监测点详细地址*/
    QString m_place;

    /*指向tableview,修改其内容*/
    QStandardItemModel *m_model;

    /*折线统计图变量*/
    qreal m_lineSeriesCount;
    QLineSeries *m_tLine;
    QLineSeries *m_hLine;

    /*统计有多少个networkthread对象*/
    static int m_total;

    /*保存json配置文件指令,与下位机通讯*/
    QByteArray m_order;
protected:
    enum statusLabel{
        connected,
        status,
        disconnected,
        error,
        reconnect
    };

    /*温湿度上下限枚举*/
    enum limit{
        tue,/*温度上限错误*/
        tle,/*温度下限错误*/
        hue,/*湿度上限错误*/
        hle/*湿度下线错误*/
    };
public:
    /*构造析构重写*/
    explicit NetworkThread(QObject *parent = nullptr);
    ~NetworkThread();
    virtual void run();

    void loadHistoryData();
    void restoreModelColor();
    /*set函数*/
    void setOrder(const QString &order);
    void setIpAndPort(const QString &ip,const ushort &port);
    void setThreadName(const QString &name);
    void setTableViewModel(QStandardItemModel *model);
    void setChartLineSeries(QLineSeries *lineSeries0,QLineSeries *lineSeries1);
    void setPlace(int monitor,QString place = "");

    /*change函数*/
    void changeTableViewTH(double temperature,double humidity);
    void changeChartTH(double temperature,double humidity,int hour,int minute);

    /*get函数*/
    const QString &getIp() const;
    const ushort &getPort() const;
    const int &getThreadTotal() const;
    const QString getCurrentTime();
    QString getDateTime(QString name = "");

    /*logic函数*/
    void cleanUpHeap();
    void whetherAlarm();

    /*send signal*/
    inline void sendStatusSignal(int networkStatus,const char *statusMessage = nullptr)
    {
        emit changeStatusLabel(networkStatus,statusMessage);
    }
public slots:
    /*slot函数*/
    void connectedSlot();
    void disconnectedSlot();
    void stateChangedSlot(QAbstractSocket::SocketState code);
    void socketErrorSlot(QAbstractSocket::SocketError code);
    void readyReadSlot();
    void dbWriteSlot();
    void reconnectSlot();
    void stopReconnectSlot();
    void threadExitSlot();
signals:
    void changeStatusLabel(int networkStatus,const char *statusMessage = nullptr);
    void tHAlarmSignal();
    void changeTableViewColorSignal(bool alarm);
    void playSoundSignal();
    void stopSoundSignal();
};
#endif //NETWORKTHREAD_H
