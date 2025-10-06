#ifndef STATISTICSLAYOUT_H
#define STATISTICSLAYOUT_H

#include <QWidget>
#include <QMessageBox>
#include <QLabel>
#include <QSqlTableModel>

#include "InterfaceLayout.h"
#include "DatabaseTool.h"
#include "StatisticalChart.h"

namespace Ui {
class StatisticsLayout;
}

class StatisticsLayout : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsLayout(QWidget *parent = nullptr);
    ~StatisticsLayout();
    void initTableView();
    void initComboBox();
    void initDataTime();
    void messageBox(QString title,QString text);
private:
    Ui::StatisticsLayout *ui;
    QSqlTableModel *m_model;
    QString m_sqlStatement;
    queryTable_t query;
private slots:
    void queryButtonSlot();
    void generateButtonSlot();
};

#endif // StatisticsLayout_H
