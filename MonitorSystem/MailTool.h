#ifndef MAILTOOL_H
#define MAILTOOL_H

#include <QObject>

class MailTool : public QObject
{
    Q_OBJECT
private:
    QString m_account;
    QString m_password;
    QString m_lastError;
    explicit MailTool(QObject *parent = nullptr);
    ~MailTool();
    static MailTool* m_self;
public:
    static MailTool* createMailTool();
    static void deleteMailTool();
    void setAccountPassword(const QString& account,const QString& password);
    bool sendMail(const QString& userAccount,const QString& title,const QString& content);
    const QString& getLastError();
signals:

public slots:
};

#endif // MAILTOOL_H
