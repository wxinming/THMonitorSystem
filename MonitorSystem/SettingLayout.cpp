#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>

#include "SettingLayout.h"
#include "ui_SettingLayout.h"
#include "JsonTool.h"

//static int g_total = 0;
                                                         /*1   2          3              5      */
QStringList SettingLayout::m_labels = QString::fromUtf8("帐号,密码,刷新时间间隔,数据库写入时间间隔,"
                                                        /*    6       7      8*/
                                                        "电子邮箱,忽略报警,字体大小,"
                                                        /*          9           10*/
                                                        "y轴温度上限刻度,y轴温度下限刻度,"
                                                        /*          11           12*/
                                                        "y轴湿度上限刻度,y轴湿度下限刻度,"
                                                        /*            13               14*/
                                                        "y轴温度报警上限刻度,y轴温度报警下限刻度,"
                                                        /*            15               16*/
                                                        "y轴湿度报警上限刻度,y轴湿度报警下限刻度").split(",");

                                     /*    1         2                          3*/
QStringList SettingLayout::m_tips = {"用户帐号","用户密码","界面刷新的时间间隔(单位:毫秒)",
                                     /*5*/
                                     "每隔多长时间写入一次数据库(单位:毫秒)",
                                     /*6*/
                                     "如果发生监控异常,发送异常信息到的电子邮箱地址",
                                     /*7*/
                                     "是否在休息期间忽略报警(0不忽略,1忽略)",
                                     /*8*/
                                     "设置表格字体大小",
                                     /*9                          10*/
                                     "y轴温度最大值范围","y轴温度最小值范围",
                                     /*11                         12*/
                                     "y轴湿度最大值范围","y轴湿度最小值范围",
                                     /*13                                 14*/
                                     "y轴温度报警最大值范围","y轴温度报警最小值范围",
                                     /*15                                 16*/
                                     "y轴湿度报警最大值范围","y轴湿度报警最小值范围"
                                     };

SettingLayout::SettingLayout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingLayout)
{
    this->ui->setupUi(this);
    this->setWindowTitle("INVO温湿度设置");
    this->setAttribute(Qt::WA_DeleteOnClose);

    m_item = nullptr;
    m_column = 0;
    QVBoxLayout *vMainLayout = new QVBoxLayout(this);
    vMainLayout->addWidget(createTabWidget());
    vMainLayout->addWidget(createButtonWidget());
    this->setLayout(vMainLayout);
}

SettingLayout::~SettingLayout()
{
    delete ui;
    delete m_treeWidget;
    delete m_tableWidget;
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

QTabWidget *SettingLayout::createTabWidget()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setObjectName("tabWidget");
    tabWidget->addTab(createWidget(tabWidget,createTableWidget()),"用户设置");
    QPushButton *addButton = new QPushButton("添加一个节点",this);
    QPushButton *delButton = new QPushButton("删除一个节点",this);
    connect(addButton,&QPushButton::clicked,[=](){
        QTreeWidgetItem *root = addTreeNode(nullptr,"N楼高清摄像头车间");
        root = addTreeNode(root,"标题","N楼高清摄像头车间");
        QTreeWidgetItem *interface = addTreeNode(root,"界面");
        for(int j = 0;j < 3;j++)
        {
            auto place = addTreeNode(interface,QString("监测点") + QString::number(m_total++));
            addTreeNode(place,"ip地址","127.0.0.1");
            addTreeNode(place,"端口","502");
            addTreeNode(place,"指令","000000000000010300000002");
        }
    });

    connect(delButton,&QPushButton::clicked,[=](){
        if (m_item == nullptr)
        {
            messageBox("提示","请选择要删除的节点");
            return;
        }

        if (m_item->parent() == nullptr)
        {
            delete m_treeWidget->takeTopLevelItem(m_treeWidget->currentIndex().row());
        }
        else
        {
            messageBox("提示","请选择主节点删除");
//            delete m_item->parent()->takeChild(m_treeWidget->currentIndex().row());
        }
        m_item = nullptr;

    });
    QWidget *widget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(addButton);
    vbox->addWidget(delButton);
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(createTreeWidget());
    hbox->addLayout(vbox);
    widget->setLayout(hbox);
    tabWidget->addTab(createWidget(tabWidget,widget),"界面设置");
    return tabWidget;
}

QTableWidget *SettingLayout::createTableWidget()
{
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(1);
    m_tableWidget->setRowCount(JsonTool::createJsonTool()->getUserKeyList().count());
    m_tableWidget->horizontalHeader()->setHidden(true);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QFont font;
    font.setFamily("宋体");
    m_tableWidget->setFont(font);
    m_tableWidget->setVerticalHeaderLabels(m_labels);

    JsonTool *jsonTool = JsonTool::createJsonTool();
    for(int i = 0;i<jsonTool->getUserKeyList().count();i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(jsonTool->getUserConfig(jsonTool->getUserKeyList().at(i)));
        item->setToolTip(m_tips[i]);
        m_tableWidget->setItem(i,0,item);

    }
    connect(m_tableWidget,&QTableWidget::itemChanged,this,&SettingLayout::tableWidgetItemChangedSlot);
    return m_tableWidget;
}

QTreeWidget *SettingLayout::createTreeWidget()
{
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setExpandsOnDoubleClick(true);
    m_treeWidget->setAnimated(true);
    m_treeWidget->setHeaderHidden(false);
    m_treeWidget->setHeaderLabels(QStringList({"键","值"}));
    m_treeWidget->setColumnCount(2);
    m_treeWidget->setColumnWidth(0,200);
    m_treeWidget->setAutoScroll(true);

    JsonTool *jsonTool = JsonTool::createJsonTool();
    tabWidgetValue_t widgetValue;

    for (int i = 0;i< jsonTool->getTabWidgetCount();i++)
    {
        jsonTool->getTabWidgetConfig(i,widgetValue);
        QTreeWidgetItem *root = addTreeNode(nullptr,widgetValue.title);
        root = addTreeNode(root,"标题",widgetValue.title);
        QTreeWidgetItem *interface = addTreeNode(root,"界面");
        for(int j = 0;j < 3;j++)
        {
            auto place = addTreeNode(interface,QString("监测点") + QString::number(m_total++));
            addTreeNode(place,"ip地址",widgetValue.value[j].ip);
            addTreeNode(place,"端口",widgetValue.value[j].port);
            addTreeNode(place,"指令",widgetValue.value[j].order);
        }
    }

    connect(m_treeWidget,&QTreeWidget::itemDoubleClicked,this,&SettingLayout::itemDoubleClickedSlot);
    connect(m_treeWidget,&QTreeWidget::itemChanged,this,&SettingLayout::itemChangedSlot);
    connect(m_treeWidget,&QTreeWidget::itemPressed,[=](QTreeWidgetItem *item,int column){
       qDebug() << "pressed" <<endl;
       m_item =  item;
       m_column = column;
    });
//    connect(m_treeWidget,&QTreeWidget::itemSelectionChanged,this,&SettingLayout::itemCollapsedSlot);
    return m_treeWidget;
}

QWidget *SettingLayout::createWidget(QWidget *parent,QWidget *widget)
{
    QWidget *userWidget = new QWidget(parent);
    QVBoxLayout *vBox = new QVBoxLayout(userWidget);
    vBox->addWidget(widget);
    userWidget->setLayout(vBox);
    return userWidget;
}

QWidget *SettingLayout::createButtonWidget()
{
    QWidget *widget = new QWidget(this);
    QPushButton *saveButton = new QPushButton(widget);
    saveButton->setText("保存");
    QPushButton *exitButton = new QPushButton(widget);
    exitButton->setText("退出");
    QHBoxLayout *buttonLayout = new QHBoxLayout(widget);
    buttonLayout->addWidget(saveButton);
//    buttonLayout->addStretch(1);
    buttonLayout->addWidget(exitButton);
    widget->setLayout(buttonLayout);

    connect(saveButton,&QPushButton::clicked,this,&SettingLayout::saveButtonSlot);

    connect(exitButton,&QPushButton::clicked,[=](){
        this->deleteLater();
    });
    return widget;
}

QTreeWidgetItem *SettingLayout::addTreeNode(QTreeWidgetItem *parent,QString name,QString value)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0,name);
    if (value != "")
        item->setText(1,value);

    if (parent)
        parent->addChild(item);
    else
        m_treeWidget->addTopLevelItem(item);
    return item;
}

bool SettingLayout::checkDigit(const QString &digit,bool isFloat)
{
    bool result;
    if (isFloat)
    {
        digit.toDouble(&result);
    }
    else
    {
        digit.toInt(&result);
    }
    return result;
}

bool SettingLayout::checkIpRegular(const QString &ip)
{
    bool result = true;
    do{
        QStringList list = ip.split(".");
        if (list.count() != 4)
        {
            result = false;
            break;
        }
        for (int i = 0;i < list.count();i++)
        {
            if (!checkDigit(list[i]) || list[i].toInt() < 0x0 || list[i].toInt() > 0xff)
            {
                result = false;
                break;
            }
        }
    }while(false);
    return result;
}

bool SettingLayout::checkPortRegular(const QString &port)
{
    bool result = true;
    do
    {
        if (!checkDigit(port))
        {
            result = false;
            break;
        }
        if (port.toInt() < 0x0 || port.toInt() > 0xffff)
        {
            result = false;
            break;
        }
    }while(false);
    return result;
}

bool SettingLayout::assertTHRegular(const QString &key)
{
    return key != "yAxisAlarmHll" && key != "yAxisAlarmHul" && key != "yAxisAlarmTll" &&
            key != "yAxisAlarmTul" && key != "yAxisHllScale" && key != "yAxisHulScale" &&
            key != "yAxisTllScale" && key != "yAxisTulScale";
}

int SettingLayout::questionBox(QString title, QString text)
{
    QMessageBox msgBox(this);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes,"确定");
    msgBox.setButtonText(QMessageBox::No,"取消");
    msgBox.resize(100,50);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setModal(false);
    return msgBox.exec();
}

void SettingLayout::messageBox(QString title,QString text)
{
    QMessageBox msgBox(this);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.setButtonText(QMessageBox::Yes,"确定");
    msgBox.resize(100,50);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setModal(false);
    msgBox.exec();
}

void SettingLayout::tableWidgetItemChangedSlot(QTableWidgetItem *item)
{
    const QString key = JsonTool::createJsonTool()->getUserKeyList().at(item->row());
    if (JsonTool::createJsonTool()->getUserConfig(key) == item->text())
        return;

    QString warning;
    do{
        if (key != "email" && key != "account" && key != "password")
        {
            if (assertTHRegular(key))
            {
                if (!checkDigit(item->text()))
                {
                    warning = m_labels[item->row()] + ",不符合规则";
                    break;
                }

                if (key == "alarmTime" || key == "buzzingTime"
                     || key == "dbWriteTime" || key == "refreshTime")
                {
                    if (item->text().toInt() < 1000)
                    {
                        warning = m_labels[item->row()] + ",时间间隔不能小于1000毫秒";
                        break;
                    }
                }

                if ((key == "fontSize") && item->text().toInt() < 0)
                {
                    warning = m_labels[item->row()] + "大小,不能为负数";
                    break;
                }
                if (key == "ignore")
                {
                    if (item->text() != "0" && item->text() != "1")
                    {
                        warning = m_labels[item->row()] + ",该值只能为0或1";
                        break;
                    }

                }
            }
            else
            {
                if (!checkDigit(item->text(),true))
                {
                    warning = m_labels[item->row()] + ",不能为字符串";
                    break;
                }
            }

        }
    }while(false);

    if (!warning.isNull())
    {
        messageBox("警告",warning);
        item->setData(0,JsonTool::createJsonTool()->getUserConfig(key));
    }
    else
    {
        JsonTool::createJsonTool()->setUserConfig(key,item->text());
    }

}

void SettingLayout::itemChangedSlot(QTreeWidgetItem *item,int column)
{

    if (item->text(0) == "ip地址")
    {
        if (!checkIpRegular(item->text(1)))
        {
            messageBox("警告","IP地址输入不符合规则");
            item->setText(column,"127.0.0.1");
        }
        else
        {
            qDebug() <<"item:text" << item->text(column) <<endl;
        }
    }

    if (item->text(0) == "端口")
    {
        if (!checkPortRegular(item->text(1)))
        {
            messageBox("警告","端口输入不符合规则");
            item->setText(column,"502");
        }
        else
        {
            qDebug() <<"item:text" << item->text(column) <<endl;
        }
    }

    qDebug() << m_treeWidget->currentIndex().row() << endl;
    m_treeWidget->closePersistentEditor(item,column);
}

void SettingLayout::itemCollapsedSlot()
{
    qDebug() << __FUNCTION__ <<endl;

    //    m_treeWidget->closePersistentEditor(m_item,m_column);
}

void SettingLayout::saveButtonSlot()
{
    if (JsonTool::createJsonTool()->updateJsonFile("config.json"))
    {
        messageBox("提示","修改成功,请重启应用程序!");
    }
    else
    {
        messageBox("提示","修改失败");
    }
}

void SettingLayout::itemDoubleClickedSlot(QTreeWidgetItem *item, int column)
{
    if (column != 0 && item->text(column) != "")
    {
        m_treeWidget->openPersistentEditor(item,column);
        m_item = item;
        m_column = column;
    }
}
