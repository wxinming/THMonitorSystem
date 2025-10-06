#ifndef JSONTOOL_H
#define JSONTOOL_H
#pragma execution_character_set("utf-8")

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QVector>

typedef struct _interfaceValue{
    QString ip;
    QString port;
    QString order;
//    QString remark;/*监测点详细地址*/
}interfaceValue_t;

typedef struct _tabWidgetValue{
    QString title;
    interfaceValue_t value[3];
}tabWidgetValue_t;

/*用于读写json文件*/
class JsonTool
{
private:
    /*构造析构*/
    JsonTool();
    ~JsonTool();

    /*保存user json对象*/
    QJsonObject m_userJsonObj;

    /*保存tabwidget json数组*/
    QJsonArray m_tabWidgetJsonVal;

    /*单例*/
    static JsonTool *self;

    /*user值*/
    QStringList m_userKeyList = {
                            "authAccount",         /*1.账号*/
                            "authPassword",        /*2.密码*/
                            "refreshTime",     /*3.界面刷新时间*/
                            "dbWriteTime",     /*5.写入数据库时间间隔*/
                            "email",           /*6.电子邮箱*/
                            "ignore",          /*7.是否忽略报警 0 1*/
                            "fontSize",        /*8.表格字体大小*/
                            "yAxisTulScale",   /*9.y轴温度上限刻度*/
                            "yAxisTllScale",   /*10.y轴温度下限刻度*/
                            "yAxisHulScale",   /*11.y轴湿度上限刻度*/
                            "yAxisHllScale",   /*12.y轴湿度下限刻度*/
                            "yAxisAlarmTul",   /*13.y轴报警温度上限*/
                            "yAxisAlarmTll",   /*14.y轴报警温度下限*/
                            "yAxisAlarmHul",   /*15.y轴报警湿度上限*/
                            "yAxisAlarmHll",    /*16.y轴报警湿度下限*/
                            "dbAccount",//17.数据库账号
                            "dbPassword"//18.数据库密码
                            };

    /*user值*/
    QStringList m_userValueList = {
        /*    1          2      3        5                 6*/
        "admin","admin123","60000","300000","p.166cd14b12d34685ebdc59479da2695a@mail.teambition.com",

        /*7   8    9  10    11   12   13   14   15   16   17  18*/
        "0","50","35","0","100","35","30","10","60","40","root","123456"
    };

public:
    JsonTool &operator=(const JsonTool &) = delete;
    JsonTool(const JsonTool &) = delete;

    /*创建JsonTool单例*/
    static JsonTool *createJsonTool();

    /*释放JsonTool单例*/
    static void deleteJsonTool();

    /*读取json配置文件*/
    bool readJsonFile(const QString &file);

    /*写入默认json配置文件*/
    bool writeJsonFile(const QString &file);

    /*更新json配置文件*/
    bool updateJsonFile(const QString &file);

    /*创建一个user json对象*/
    QJsonObject createUserObj(const QStringList &val);

    /*创建一个tabwidget json数组对象值 重载函数*/
    QJsonObject createTabWidgetVal(const QString &title,const QVector<QStringList> &vsl);
    QJsonObject createTabWidgetVal(const tabWidgetValue_t &value);

    /*打印user json对象*/
    void printUser(const QJsonObject &obj) const;

    /*打印tabwidget json对象*/
    void printTabWidget(const QJsonValue &obj) const;

    /*设置user json对象*/
    void setUserConfig(const QString &key,const QString &value);

    /*设置tabwidget json对象*/
    bool setTabWidgetConfig(int number,tabWidgetValue_t &config);

    /*获取user json对象值*/
    QString getUserConfig(const QString &name) const;

    /*获取tabwidget json数组,元素个数*/
    int getTabWidgetCount() const;

    /*获取tabwidget json对象数组值 重载函数*/
    QString getTabWidgetConfig(int number,const char *name,
                                     int number1 = 0,const char *name1 = nullptr) const;
    bool getTabWidgetConfig(int number,tabWidgetValue_t &config);

    /*获取用户keylist*/
    QStringList getUserKeyList() const;
};

#endif // JSONTOOL_H
