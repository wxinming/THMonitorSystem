#include "MailTool.h"
#include "sendemailapi/smtpmime.h"

MailTool* MailTool::m_self = nullptr;
MailTool::MailTool(QObject *parent) : QObject(parent)
{

}

MailTool::~MailTool()
{

}

MailTool *MailTool::createMailTool()
{
    if (!m_self)
    {
        m_self = new MailTool;
    }
    return m_self;
}

void MailTool::deleteMailTool()
{
    if (m_self)
    {
        delete m_self;
        m_self = nullptr;
    }
}

void MailTool::setAccountPassword(const QString& account,const QString& password)
{
    m_account = account;
    m_password = password;
}

bool MailTool::sendMail(const QString& userAccount,const QString& title,const QString& text)
{
//    SmtpClient smtp("smtp.163.com", 25, SmtpClient::TcpConnection);

    SmtpClient smtp("smtp.qiye.163.com", 25, SmtpClient::TcpConnection);

    smtp.setUser(m_account);
    smtp.setPassword(m_password);

    //构建邮件主题,包含发件人收件人附件等.
    MimeMessage message;
    message.setSender(new EmailAddress(m_account));

    //逐个添加收件人
    message.addRecipient(new EmailAddress(userAccount));

    //构建邮件标题
//    message.setSubject(QStringLiteral(title));
    message.setSubject((title));
    //构建邮件正文
    MimeText mimeText;
    mimeText.setText(text);
    message.addPart(&mimeText);

    bool result = false;
    do
    {
        if (!smtp.connectToHost()){
            m_lastError = "连接错误";
            break;
        }
        if (!smtp.login()){
            m_lastError = "登录错误";
            break;
        }
        if (!smtp.sendMail(message)){
            //m_lastError = "发送错误";
            break;
        }
        result = true;
    }while(false);
    if (result) smtp.quit();
    return result;
}

const QString &MailTool::getLastError()
{
    return m_lastError;
}
