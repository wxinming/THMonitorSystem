#ifndef INTERFACELAYOUT_H
#define INTERFACELAYOUT_H

#include <QWidget>
#include <QtCharts>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QTcpSocket>
#include <QDebug>
#include <QTimer>

#include "NetworkThread.h"

using namespace QtCharts;

namespace Ui {
class InterfaceLayout;
}

class InterfaceLayout : public QWidget
{
    Q_OBJECT
private:
    Ui::InterfaceLayout *ui;
    static const QString m_sheet;
    /*统计有多少个对象*/
    static int m_total;

    static int m_monitor;
    /*检测开关标识符0为未按下,1为按下*/
    bool m_switch;

    /*状态标签闪烁*/
    QTimer m_errorTimer;
    QTimer m_errorTimer1;

    /*网络处理线程*/
    NetworkThread *m_networkThread;

    /*tableview的model用于修改值*/
    QStandardItemModel *m_model;
protected:
    /*状态标签枚举*/
    enum statusLabel{
        connected,/*连接成功*/
        status,/*连接中*/
        disconnected,/*断开连接*/
        error,/*连接错误*/
        reconnect,
        shutdown/*已弃用*/
    };
public:
    /*构造析构*/
    explicit InterfaceLayout(QWidget *parent = nullptr);
    ~InterfaceLayout();

    /*init函数*/
    void initUi();
    void initTableView();
    void initChart();
    void initNetworkThread(const QString &ip,const ushort &port,const QString &order);

    static int getTotal();

    NetworkThread *getNetworkThread();
    /*set函数*/
    void setStatusLabelLogic(int networkStatus,const char *statusMessage = nullptr);
    void setControlLabelShow(bool onOff);
public slots:
    /*slot函数*/
    void monitoringButtonSlot();
    void errorTimerSlot();
    void errorTimer1Slot();
    void recvStatusSlot(int,const char *);
    void oneKeyStartSlot();
    void oneKeyStopSlot();
    void changeTableViewColorSlot(bool alarm);
signals:
    inline void stopReconnectTimerSignal();

};
#endif //INTERFACELAYOUT_H
