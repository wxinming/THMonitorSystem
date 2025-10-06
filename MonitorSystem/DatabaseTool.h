#ifndef DATABASETOOL_H
#define DATABASETOOL_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include <QSqlTableModel>
#include <QtCharts>
#include "JsonTool.h"

typedef struct InsertTable{
    int monitor;
    double temperature;
    double humidity;
    QString state;
}insertTable_t;

typedef struct QueryTable{
    int monitor;
    QString startDateTime;
    QString endDateTime;
    int result;
}queryTable_t;

class DatabaseTool
{
public:
    DatabaseTool(const DatabaseTool &) = delete;

    DatabaseTool &operator=(const DatabaseTool &) = delete;

    QSqlDatabase &getSqlDatabase() const;

    static DatabaseTool *createDatabase();

    static void deleteDatabaseTool();

    bool connectDatabase();

    bool createTable();

    bool insertData(insertTable_t &value);

    bool queryData(QueryTable &value);

    QString getLastError() const;

    void setModel(QSqlTableModel *model);

    void setState(bool isOpen);

    bool getState();
private:
    QString m_lastError;

    QSqlDatabase m_database;

    DatabaseTool();

    ~DatabaseTool();

    static DatabaseTool *m_self;

    QSqlTableModel *m_model = nullptr;

    bool m_isOpen = true;

    bool m_queryResult = false;
};

#endif // DATABASETOOL_H
