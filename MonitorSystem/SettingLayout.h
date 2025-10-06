#ifndef SETTINGLAYOUT_H
#define SETTINGLAYOUT_H

#include <QWidget>
#include <QTableWidget>
#include <QTreeWidget>

namespace Ui {
class SettingLayout;
}

class SettingLayout : public QWidget
{
    Q_OBJECT

private:
    Ui::SettingLayout *ui;
    QTreeWidget *m_treeWidget;
    QTableWidget *m_tableWidget;
    QTreeWidgetItem *m_item;
    int m_column;
    static QStringList m_labels;
    static QStringList m_tips;
    int m_total = 1;
public:
    explicit SettingLayout(QWidget *parent = nullptr);
    ~SettingLayout();

    /*create函数*/
    QTabWidget *createTabWidget();
    QTableWidget *createTableWidget();
    QTreeWidget *createTreeWidget();
    QWidget *createWidget(QWidget *parent,QWidget *widget);
    QWidget *createButtonWidget();
    QTreeWidgetItem *addTreeNode(QTreeWidgetItem *parent,QString name,QString value = "");

    bool checkDigit(const QString &digit,bool isFloat = false);
    bool checkIpRegular(const QString &ip);
    bool checkPortRegular(const QString &port);
    bool assertTHRegular(const QString &key);
    int questionBox(QString title, QString text);
    void messageBox(QString title,QString text);
signals:
    void exitApplicationSignal();
private slots:
    /*slot*/
    void tableWidgetItemChangedSlot(QTableWidgetItem *item);
    void itemDoubleClickedSlot(QTreeWidgetItem *item,int column);
    void itemChangedSlot(QTreeWidgetItem *item,int column);
    void itemCollapsedSlot();
    void saveButtonSlot();
};

#endif // SETTINGLAYOUT_H
