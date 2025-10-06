#include <QVBoxLayout>

#include "StatisticsLayout.h"
#include "ui_StatisticsLayout.h"

StatisticsLayout::StatisticsLayout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsLayout)
{
    ui->setupUi(this);
    this->setWindowTitle("INVO温湿度监控统计");
    this->setAttribute(Qt::WA_DeleteOnClose);
    initComboBox();
    initTableView();
    initDataTime();
    connect(ui->queryButton,&QPushButton::clicked,this,&StatisticsLayout::queryButtonSlot);
    connect(ui->generateButton,&QPushButton::clicked,this,&StatisticsLayout::generateButtonSlot);
    if (!DatabaseTool::createDatabase()->getState())
    {
        ui->queryButton->setEnabled(false);
        ui->generateButton->setEnabled(false);
    }
}

StatisticsLayout::~StatisticsLayout()
{
    delete ui;
    delete m_model;
#ifndef NDEBUG
    qDebug() << __FUNCTION__ << endl;
#endif
}

void StatisticsLayout::initTableView()
{
    m_model = new QSqlTableModel(ui->recordTableView,DatabaseTool::createDatabase()->getSqlDatabase());
//    m_model->setHorizontalHeaderLabels({"监测点","时间","温度","湿度","状态"});
    m_model->setTable("record");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
//    m_model->select();
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "监测点");
    m_model->setHeaderData(2, Qt::Horizontal, "时间");
    m_model->setHeaderData(3, Qt::Horizontal, "温度");
    m_model->setHeaderData(4, Qt::Horizontal, "湿度");
    m_model->setHeaderData(5, Qt::Horizontal, "状态");

    ui->recordTableView->setShowGrid(true);
    ui->recordTableView->setGridStyle(Qt::SolidLine);
    ui->recordTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->recordTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->recordTableView->verticalHeader()->setHidden(true);
    ui->recordTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->recordTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->recordTableView->verticalHeader()->setDefaultSectionSize(15);
    ui->recordTableView->setModel(m_model);
}

void StatisticsLayout::initComboBox()
{
    ui->comboBox->addItem("全部");
    for(int i = 0;i<InterfaceLayout::getTotal();i++)
    {
        ui->comboBox->addItem("监测点" + QString::number(i + 1));
    }
//    ui->startDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
}

void StatisticsLayout::initDataTime()
{
    QDateTime endDateTime = QDateTime::currentDateTime();

    QDateTime startDateTime;
    QDate date;
    date.setDate(2019,1,1);
    startDateTime.setDate(date);
    QTime time;
    time.setHMS(0,0,0);
    startDateTime.setTime(time);

    ui->startDateTimeEdit->setDateTime(startDateTime);
    ui->endDateTimeEdit->setDateTime(endDateTime);
}

void StatisticsLayout::queryButtonSlot()
{
    query.startDateTime = ui->startDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    query.endDateTime = ui->endDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    bool isChecked = ui->DateCheckBox->isChecked();

    if (ui->comboBox->currentText() != "全部")
    {
        query.monitor = ui->comboBox->currentText().mid(3).toInt();
        if (isChecked)
        {
            m_model->setFilter(QString("monitor=%1 and DateTime between \"%2\" and \"%3\"")
                               .arg(query.monitor).arg(query.startDateTime).arg(query.endDateTime));

        }
        else
        {
            m_model->setFilter(QString("monitor=%1").arg(query.monitor));
        }
    }
    else
    {
        if (isChecked)
        {
            m_model->setFilter(QString("DateTime between \"%2\" and \"%3\"")
                               .arg(query.startDateTime).arg(query.endDateTime));
        }
    }
    bool result = m_model->select();

    if (!result)
    {
        messageBox("提示","查询错误");
        return;
    }
    int rowCount = m_model->rowCount();
    if (!rowCount)
    {
        messageBox("提示","没有查询到符合条件的数据");
    }
}

void StatisticsLayout::generateButtonSlot()
{
    if (ui->comboBox->currentText() == "全部")
    {
        messageBox("提示","生成的监测点不能为全部!");
        return;
    }

    if (!ui->DateCheckBox->isChecked())
    {
        messageBox("提示","请选择要生成的日期范围!");
        return;
    }

    int monitor = ui->comboBox->currentText().mid(3).toInt();
    QString startDateTime = ui->startDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    QString endDateTime = ui->endDateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
    StatisticalChart *chart = new StatisticalChart(monitor,startDateTime,endDateTime);
    chart->show();
}

void StatisticsLayout::messageBox(QString title,QString text)
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
