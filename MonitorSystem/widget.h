#ifndef WIDGET_H
#define WIDGET_H

#include <windows.h>
#include <QWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QSoundEffect>
#include <QTimer>

#include "DatabaseTool.h"
#include "InterfaceLayout.h"


#include "JsonTool.h"
#include "SettingLayout.h"
#include "LoginLayout.h"
#include "StatisticsLayout.h"
#include "AlarmSound.h"
#include "AlgorithmTool.h"
#include "MailTool.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
private:
    Ui::Widget *ui;

    /*读写json配置文件类*/
    JsonTool *m_jsonTool;
    QTabWidget *m_tabWidget;
    QLabel *m_titleLabel;
    QLabel *m_timeLabel;
    DatabaseTool *m_database;
    MailTool *m_mailTool;
    int m_width = 0;
    int m_height = 0;
    QTimer m_switchTimer;
public:
    /*构造析构*/
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    /*create函数*/
    QWidget *createInterfaceLayout(JsonTool *&);
    QLayout *createMainLayout();

    /*init函数*/
    void initUi();
    void initJson();
    void initDatabase();

    void messageBox(QString title,QString text);
    int questionBox(QString title,QString text);
private slots:
    void loginSuccessSlot();
    void exitApplicationSlot();
signals:
    void oneKeyStartSignal();
    void oneKeyStopSignal();
    void muteSoundSignal();
    void unmuteSoundSignal();
};

#endif // WIDGET_H
