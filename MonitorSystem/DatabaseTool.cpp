#include "DatabaseTool.h"

DatabaseTool *DatabaseTool::m_self = nullptr;

DatabaseTool::DatabaseTool()
{

}

DatabaseTool::~DatabaseTool()
{
    if (m_database.isOpen())
    {
        m_database.close();
    }
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

QSqlDatabase &DatabaseTool::getSqlDatabase() const
{
    return const_cast<QSqlDatabase&>(m_database);
}

void DatabaseTool::deleteDatabaseTool()
{
    if (m_self)
    {
        delete m_self;
        m_self = nullptr;
    }
}

DatabaseTool *DatabaseTool::createDatabase()
{
    if (!m_self)
    {
        m_self = new DatabaseTool;
    }
    return m_self;
}

bool DatabaseTool::connectDatabase()
{
    bool result = true;
    do{

        const QString dbAccount = JsonTool::createJsonTool()->getUserConfig("dbAccount");
        const QString dbPassword = JsonTool::createJsonTool()->getUserConfig("dbPassword");


        m_database = QSqlDatabase::addDatabase("QMYSQL");
        m_database.setHostName("localhost");
        m_database.setUserName(dbAccount);
        m_database.setPassword(dbPassword);
        if (!m_database.open())
        {
            m_lastError = m_database.lastError().driverText();
            result = false;
            break;
        }
        QSqlQuery query(m_database);
        if (!query.exec("create database if not exists mylog charset=utf8;"))
        {
            result = false;
            break;
        }
        m_database.close();
        m_database.setDatabaseName("mylog");
        m_database.open();
    }while (false);
    return result;
}

bool DatabaseTool::createTable()
{
    bool result = true;
    QString table = "create table if not exists record(id int primary key auto_increment,"
                    "monitor int not null,"
                    "dateTime timestamp default current_timestamp,"
                    "temperature float not null,"
                    "humidity float not null,"
                    "state varchar(10) not null);";

    QSqlQuery query(m_database);
    if (!query.exec(table))
    {
        m_lastError = query.lastError().driverText();
        result = false;
    }
    return result;
}

bool DatabaseTool::insertData(insertTable_t &value)
{
    bool result = true;
    QString str = QString("insert into record values(null,%1,null,%2,%3,'%4');").arg(value.monitor)
            .arg(value.temperature).arg(value.humidity).arg(value.state);

    QSqlQuery query(m_database);
    if (!query.exec(str))
    {
        m_lastError = query.lastError().driverText();
        result = false;
    }
    return result;
}

bool DatabaseTool::queryData(QueryTable &value)
{
    bool result = true;
    QString str;
    if (value.startDateTime.isEmpty() && value.endDateTime.isEmpty())
    {
        str = QString("select * from record where monitor=%1;").arg(value.monitor);
    }
    else
    {
        str = QString("select * from record where monitor=%1 and dateTime between \"%2\" and \"%3\";")
                .arg(value.monitor).arg(value.startDateTime).arg(value.endDateTime);
    }

    do{
        QSqlQuery query(m_database);
        if (!query.exec(str))
        {
            m_lastError = query.lastError().driverText();
            result = false;
            break;
        }
        int i = 0;

        while (query.next()) {
            i++;
        }
        value.result = i;
    }while(false);
    return result;
}

QString DatabaseTool::getLastError() const
{
    return m_lastError;
}

void DatabaseTool::setModel(QSqlTableModel *model)
{
    m_model = model;
}

void DatabaseTool::setState(bool isOpen)
{
    m_isOpen = isOpen;
}

bool DatabaseTool::getState()
{
    return m_isOpen;
}
