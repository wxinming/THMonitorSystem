#include <QFileInfo>

#include "widget.h"
#include "ui_widget.h"
#include "MailTool.h"

/*每个tabwidget中添加最多3个interfaceLayout*/
#define NUMBER 3

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    //m_width = GetSystemMetrics(SM_CXSCREEN);
    //m_height = GetSystemMetrics(SM_CYSCREEN);
//    this->resize(m_width - 50,m_height - 80);
    this->resize(m_width - 80, m_height - 100);
    this->setAttribute(Qt::WA_DeleteOnClose);

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(palette);
    connect(this,&Widget::muteSoundSignal,AlarmSound::createAlarmSound(),&AlarmSound::muteSoundSlot);
    connect(this,&Widget::unmuteSoundSignal,AlarmSound::createAlarmSound(),&AlarmSound::unmuteSoundSlot);

    /*初始化JSON*/
    initJson();

    /*初始化数据库*/
    initDatabase();

    /*初始化UI*/
    initUi();

    /*创建主布局*/
    this->setLayout(createMainLayout());

    m_mailTool = MailTool::createMailTool();
    m_mailTool->setAccountPassword("tb@invo.cn","invo.88812699");
#if 0
    QString mailTitle;
    if (1 && 1)
        mailTitle = QString("位置:%1 温湿度超标(%2℃,%3%)").arg(1).arg(30.2).arg(52.2);
    else if (1)
        mailTitle = QString("位置:%1 温度超标(%2℃)").arg(1).arg(30.2);
    else if (1)
        mailTitle = QString("位置:%1 湿度超标(%2%)").arg(1).arg(52.2);

    int monitor;
    if (2 % 3 == 0)
    {
        monitor = 2 / 3;
    }
    else
    {
        monitor = 2 / 3 + 1;
    }
    QString title = m_jsonTool->getTabWidgetConfig(monitor - 1,"title");
    qDebug() << "title" << title << endl;
    QString content = QString("超标位置:\n   %1,%2号探头;\n"
                              "超标情况:\n   温湿度规格:温度(%3℃~%4℃),湿度(%5%~%6%);\n   实测:温度(%7℃),湿度(%8%);")
             .arg(title).arg(0).arg(20).arg(30).arg(40).arg(60).arg(23.3).arg(50.5);
    bool result = m_mailTool->sendMail("p.166cd14b12d34685ebdc59479da2695a@mail.teambition.com",mailTitle,content);
    if (!result)
    {
        qDebug() << m_mailTool->getLastError() << endl;
    }
#endif
}

Widget::~Widget()
{
    delete ui;
    delete m_tabWidget;
    delete m_titleLabel;
    JsonTool::deleteJsonTool();
    DatabaseTool::deleteDatabaseTool();
    AlgorithmTool::deleteAlgorithmTool();
    MailTool::deleteMailTool();
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

void Widget::messageBox(QString title,QString text)
{
    QMessageBox msgBox(this);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.resize(100,50);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setModal(false);
    msgBox.exec();
}

int Widget::questionBox(QString title, QString text)
{
    QMessageBox msgBox(this);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.resize(100,50);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setModal(false);
    return msgBox.exec();
}

/*初始化UI*/
void Widget::initUi()
{
    /*设置界面背景*/
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background,Qt::black);
    this->setPalette(palette);

    /*设置主标题*/
    m_titleLabel = new QLabel(this);
    QImage *image = new QImage;
    if (image->load(":/images/resources/images/Log.png"))
    {
        QPixmap pixMap = QPixmap::fromImage(*image)
                .scaled(image->width() / 7,image->height() / 7,
                        Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_titleLabel->setPixmap(pixMap);
    }
    else
    {
        m_titleLabel->setText("INVO温湿度监控系统");
    }

    m_titleLabel->setStyleSheet("background-color:rgb(255, 127, 0);font-size:60px;color:white;font-family:隶书");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    /*设置tabwidget*/
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setObjectName(QString::fromUtf8("m_tabWidget"));
    m_tabWidget->setStyleSheet("background-color:rgb(0,0,0)");

    int tabCount = m_jsonTool->getTabWidgetCount();
    for(int i = 0;i< tabCount;i++)
    {
        /*interfacelayout添加到tabwidget中*/
        m_tabWidget->addTab(createInterfaceLayout(m_jsonTool),m_jsonTool->getTabWidgetConfig(i,"title"));
    }

    if (tabCount > 1)
    {
        connect(&m_switchTimer,&QTimer::timeout,this,[=](){
            static int count = 0;
            m_tabWidget->setCurrentIndex(count++);
            if (count >= m_jsonTool->getTabWidgetCount())
            {
                count = 0;
            }
        });
    }
    m_switchTimer.start(10000);
}

void Widget::initJson()
{
    /*获取json数据*/
    m_jsonTool = JsonTool::createJsonTool();

    /*如果不存在配置文件,则创建默认参数配置文件*/
    if (!QFileInfo("config.json").exists())
    {
        m_jsonTool->writeJsonFile("config.json");
    }

    if (!m_jsonTool->readJsonFile("config.json"))
    {
        messageBox("警告","打开配置文件失败");
    }
}

void Widget::initDatabase()
{
    bool result = true;
    m_database = DatabaseTool::createDatabase();
    do
    {
        if (!m_database->connectDatabase())
        {
            result = false;
            break;
        }
        if (!m_database->createTable())
        {
            result = false;
            break;
        }

//        insertTable_t insert;
//        insert.monitor = 1;
//        insert.temperature = 35.6;
//        insert.humidity = 56.3;
//        insert.state = "正常";
//        for(int i = 0;i< 10000;i++)
//        if(!m_database->insertData(insert))
//        {
//            break;
//        }
    }while(false);

    if (!result)
    {
        int question = questionBox("警告",
                              "初始化数据库失败,原因:" + m_database->getLastError() + ",\n是否要继续启动程序?");
        if (question == QMessageBox::No)
        {
            exit(-1);
        }
        else
        {
            m_database->setState(false);
        }
    }
}

/*创建主布局*/
QLayout *Widget::createMainLayout()
{
    /*主界面布局*/
    QVBoxLayout *mainVBox0 = new QVBoxLayout(this);
    mainVBox0->addWidget(m_titleLabel);
    mainVBox0->addWidget(m_tabWidget);

    /*按钮水平布局*/
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *enterButton = new QPushButton("进入监控统计",this);
    QPushButton *muteButton = new QPushButton(this);
    QPushButton *settingButton = new QPushButton("设置",this);
    QPushButton *fullButton = new QPushButton("进入全屏",this);
    QPushButton *startButton = new QPushButton("一键启动",this);
    fullButton->setIcon(QIcon(":/images/resources/images/EnterFull.png"));
    startButton->setIcon(QIcon(":/images/resources/images/Launch.png"));

    enterButton->setMinimumWidth(900);
    buttonLayout->addWidget(enterButton);
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(fullButton);
    buttonLayout->addWidget(muteButton);
    buttonLayout->addWidget(settingButton);

    settingButton->setIcon(QIcon(":/images/resources/images/Settings.png"));
    connect(settingButton,&QPushButton::clicked,[=](){
        LoginLayout *login = new LoginLayout;
        connect(login,&LoginLayout::LoginSuccessSignal,this,&Widget::loginSuccessSlot);
        login->show();
    });

    enterButton->setIcon(QIcon(":/images/resources/images/Total.png"));
    connect(enterButton,&QPushButton::clicked,[=](){
        StatisticsLayout *totalLayout = new StatisticsLayout;
        totalLayout->show();
    });

    muteButton->setText("音量");
    muteButton->setIcon(QIcon(":/images/resources/images/Unmute.png"));
    connect(muteButton,&QPushButton::clicked,[=](){
        static bool button_switch = true;
        if (button_switch)
        {
            muteButton->setIcon(QIcon(":/images/resources/images/Mute.png"));
            emit muteSoundSignal();
        }
        else
        {
            muteButton->setIcon(QIcon(":/images/resources/images/Unmute.png"));
            emit unmuteSoundSignal();
        }
        button_switch = !button_switch;
    });

    connect(fullButton,&QPushButton::clicked,[=](){
        static bool isFull = false;
        if (!isFull)
        {
            fullButton->setText("退出全屏");
            fullButton->setIcon(QIcon(":/images/resources/images/ExitFull.png"));
            isFull = true;
            this->showFullScreen();
        }
        else
        {
            fullButton->setText("进入全屏");
            fullButton->setIcon(QIcon(":/images/resources/images/EnterFull.png"));
            isFull = false;
            this->showNormal();
        }
    });

    connect(startButton,&QPushButton::clicked,this,[=](){
        static bool isStart = false;
        if (!isStart)
        {
            emit oneKeyStartSignal();
            startButton->setText("一键关闭");
            isStart = true;
        }
        else
        {
            emit oneKeyStopSignal();
            startButton->setText("一件开启");
            isStart = false;
        }
    });
    /*按钮布局添加到主布局*/
    mainVBox0->addLayout(buttonLayout);
    mainVBox0->setMargin(1);
    return mainVBox0;
}

/*创建interface布局*/
QWidget *Widget::createInterfaceLayout(JsonTool *&json)
{
    /*统计json数组tabWidget下标*/
    static int tabWidgetId = 0;

    QWidget *widget = new QWidget(this);
    QVBoxLayout *vBoxLayout = new QVBoxLayout(widget);
    InterfaceLayout *interfaceLayout = new InterfaceLayout[NUMBER];
    for(int i = 0;i<NUMBER;i++)
    {
        interfaceLayout[i].initNetworkThread(json->getTabWidgetConfig(tabWidgetId,"interface",i,"ip"),
                                             json->getTabWidgetConfig(tabWidgetId,"interface",i,"port").toUShort(),
                                             json->getTabWidgetConfig(tabWidgetId,"interface",i,"order"));
        interfaceLayout[i].setParent(m_tabWidget);
        connect(this,&Widget::oneKeyStartSignal,interfaceLayout + i,&InterfaceLayout::oneKeyStartSlot);
        connect(this,&Widget::oneKeyStopSignal,interfaceLayout + i,&InterfaceLayout::oneKeyStopSlot);
        vBoxLayout->addWidget(interfaceLayout + i);
    }
    widget->setLayout(vBoxLayout);
    /*创建一个widget增加到tabwidget中下标自增*/
    tabWidgetId++;
    return widget;
}

/*登录成功槽*/
void Widget::loginSuccessSlot()
{
    /*创建登录,设置,统计界面类对象*/
    /*设置界面类*/
    SettingLayout *settingLayout = new SettingLayout;
    connect(settingLayout,&SettingLayout::exitApplicationSignal,this,&Widget::exitApplicationSlot);
    settingLayout->show();
}

void Widget::exitApplicationSlot()
{
    QCoreApplication::quit();
}
