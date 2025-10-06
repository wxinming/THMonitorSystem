#include "LoginLayout.h"
#include "ui_LoginLayout.h"
#include "JsonTool.h"

#include <QMessageBox>

LoginLayout::LoginLayout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginLayout)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog);

    this->setFixedSize(200,150);

    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->passwordEdit->setEchoMode(QLineEdit::Password);

    connect(ui->loginButton,&QPushButton::clicked,this,&LoginLayout::logicHandler);

    connect(ui->returnButton,&QPushButton::clicked,[=](){this->deleteLater();});
}

LoginLayout::~LoginLayout()
{
    delete ui;
    qDebug() << __FUNCTION__ << endl;
}

void LoginLayout::messageBox(QString title,QString text)
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

void LoginLayout::logicHandler()
{
    if ((ui->userNameEdit->text() == JsonTool::createJsonTool()->getUserConfig("account")) &&
            (ui->passwordEdit->text() == JsonTool::createJsonTool()->getUserConfig("password")))
    {
        sendLoginSuccessSignal();
        this->deleteLater();
    }
    else
    {
        messageBox("提示","帐号或密码输入错误，请重新输入");
    }
}
