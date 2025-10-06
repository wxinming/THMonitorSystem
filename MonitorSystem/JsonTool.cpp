#include "JsonTool.h"

#include <QMetaEnum>

#define NUMBER 3

JsonTool *JsonTool::self = nullptr;
JsonTool::JsonTool()
{
}

JsonTool::~JsonTool()
{
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

JsonTool *JsonTool::createJsonTool()
{
    if (!self)
    {
        self = new JsonTool;
    }
    return self;
}

void JsonTool::deleteJsonTool()
{
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
    if (self)
    {
        delete self;
        self = nullptr;
    }
}

bool JsonTool::readJsonFile(const QString &file)
{
    bool result = true;
    do{
        QFile loadFile(file);
        if (!loadFile.open(QIODevice::ReadOnly))
        {
            result = false;
            break;
        }

        QByteArray allData = loadFile.readAll();
        loadFile.close();

        QJsonParseError jsonError;
        QJsonDocument jsonDoc(QJsonDocument::fromJson(allData,&jsonError));

        if (jsonError.error != QJsonParseError::NoError)
        {
            QMessageBox::information(nullptr,"错误",jsonError.errorString());
            result = false;
            break;
        }

        QJsonObject rootObj = jsonDoc.object();

        if (rootObj.contains("user"))
        {
            m_userJsonObj = rootObj.value("user").toObject();
//            printUser(m_userJsonObj);
        }

        if (rootObj.contains("tabWidget"))
        {
            m_tabWidgetJsonVal = rootObj.value("tabWidget").toArray();
//            printTabWidget(m_tabWidgetJsonVal);
        }
    }while(false);

    return result;
}

bool JsonTool::writeJsonFile(const QString &file)
{
    bool result = true;
    do{
        QJsonObject rootObj;
        /*创建user对象*/
        rootObj.insert("user",createUserObj(m_userValueList));

        QJsonArray tabWidgetArray;
        QString title = "一楼高清摄像头车间";
        QVector<QStringList> vsl;
        vsl.push_back({"127.0.0.1","502","000000000000010300000002"});
        vsl.push_back({"127.0.0.1","502","000000000000010300000002"});
        vsl.push_back({"127.0.0.1","502","000000000000010300000002"});

//        QMap<QString,QString> valueMap;
//        valueMap.insert("ip","127.0.0.1");
//        valueMap.insert("port","502");
//        valueMap.insert("order","000000000000010300000002");
//        valueMap.insert("remark","监测点详细地址");
        /*创建tabwidget数组值*/
        tabWidgetArray.insert(0,createTabWidgetVal(title,vsl));
        rootObj.insert("tabWidget",tabWidgetArray);

        QJsonDocument jsonDoc(rootObj);
        QByteArray data = jsonDoc.toJson();

        QFile loadFile(file);
        if (!loadFile.open(QIODevice::WriteOnly))
        {
            result = false;
            break;
        }
        loadFile.write(data);
        loadFile.close();
    }while(false);

    return result;
}

bool JsonTool::updateJsonFile(const QString &file)
{
    QJsonObject rootObj;

    rootObj.insert("user",m_userJsonObj);
    rootObj.insert("tabWidget",m_tabWidgetJsonVal);
    QJsonDocument jsonDoc(rootObj);
    QByteArray data = jsonDoc.toJson();
    QFile loadFile(file);
    if(!loadFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return false;
    }
    loadFile.write(data);
    loadFile.close();
    return true;
}

QJsonObject JsonTool::createUserObj(const QStringList &valueList)
{
    QJsonObject userObj;
    for (int i = 0;i < m_userKeyList.count();i++)
    {
        userObj.insert(m_userKeyList[i],valueList[i]);
    }
    return userObj;
}

QJsonObject JsonTool::createTabWidgetVal(const QString &title, const QVector<QStringList> &vsl)
{
    QJsonArray interface;
    for(int i = 0;i<vsl.count();i++)
    {
        QJsonObject interfaceVal;
        interfaceVal.insert("ip",vsl.at(i).at(0));
        interfaceVal.insert("port",vsl.at(i).at(1));
        interfaceVal.insert("order",vsl.at(i).at(2));
        interface.insert(i,interfaceVal);
    }

    QJsonObject tabWidgetVal;
    tabWidgetVal.insert("title",title);
    tabWidgetVal.insert("interface",interface);
    return tabWidgetVal;
}

QJsonObject JsonTool::createTabWidgetVal(const tabWidgetValue_t &value)
{
    QJsonArray interface;
    for(int i = 0;i < NUMBER;i++)
    {
        QJsonObject interfaceVal;
        interfaceVal.insert("ip",value.value[i].ip);
        interfaceVal.insert("port",value.value[i].port);
        interfaceVal.insert("order",value.value[i].order);
//        interfaceVal.insert("remark",value.value[i].remark);
        interface.insert(i,interfaceVal);
    }

    QJsonObject tabWidgetVal;
    tabWidgetVal.insert("title",value.title);
    tabWidgetVal.insert("interface",interface);
    return tabWidgetVal;
}

void JsonTool::printUser(const QJsonObject &obj) const
{
    for(int i = 0;i<m_userKeyList.count();i++)
    {
        qDebug() <<m_userKeyList[i] <<":"<<obj[m_userKeyList[i]].toString() <<endl;
    }
}

void JsonTool::printTabWidget(const QJsonValue &jsonValue) const
{
   for (int i = 0;i < jsonValue.toArray().count();i++)
    {
       QJsonObject jsonObj = jsonValue.toArray().at(i).toObject();
       qDebug() << jsonObj["title"].toString()<<endl;
       if (jsonObj.contains("interface"))
       {
           QJsonValue jsonValue = jsonObj.value("interface");
           for (int i = 0;i< jsonValue.toArray().count();i++)
           {
               QJsonObject jsonObj = jsonValue.toArray().at(i).toObject();
               qDebug() << jsonObj["ip"].toString() << endl;
               qDebug() << jsonObj["port"].toString() << endl;
               qDebug() << jsonObj["order"].toString() <<endl;
           }
       }
   }
}

void JsonTool::setUserConfig(const QString &key, const QString &value)
{
    m_userJsonObj[key] = value;
}

bool JsonTool::setTabWidgetConfig(int number, tabWidgetValue_t &config)
{
    if (number > m_tabWidgetJsonVal.count() || number < 0)
        return false;
    m_tabWidgetJsonVal.replace(number,createTabWidgetVal(config));
//    printTabWidget(m_tabWidgetJsonVal);
    return true;
}

QString JsonTool::getTabWidgetConfig(int number,const char *name,int number1,const char *name1) const
{
    QString value;
    QJsonObject jsonObj = m_tabWidgetJsonVal.at(number).toObject();
    if (!number1 && !name1)
    {
        value = jsonObj[name].toString();
    }
    else
    {
        /*interface*/
        if (jsonObj.contains(name))
        {
            QJsonValue jsonValue = jsonObj.value("interface");
            QJsonObject jsonObj = jsonValue.toArray().at(number1).toObject();
            value =  jsonObj[name1].toString();
        }
    }
    return value;
}

bool JsonTool::getTabWidgetConfig(int number, tabWidgetValue_t &config)
{
    if (number > m_tabWidgetJsonVal.count() || number < 0)
        return false;

    QJsonObject tabWidgetValue =  m_tabWidgetJsonVal.at(number).toObject();
    config.title = tabWidgetValue["title"].toString();
    if (tabWidgetValue.contains("interface"))
    {
        QJsonValue jsonValue = tabWidgetValue.value("interface");

        for(int i = 0;i<NUMBER;i++)
        {
            QJsonObject jsonObj = jsonValue.toArray().at(i).toObject();

            config.value[i].ip = jsonObj["ip"].toString();
            config.value[i].port = jsonObj["port"].toString();
            config.value[i].order = jsonObj["order"].toString();
        }
    }
    return true;
}

QStringList JsonTool::getUserKeyList() const
{
    return m_userKeyList;
}

QString JsonTool::getUserConfig(const QString &name) const
{
    return m_userJsonObj[name].toString();
}

int JsonTool::getTabWidgetCount() const
{
    return m_tabWidgetJsonVal.count();
}
