#ifndef LOGINLAYOUT_H
#define LOGINLAYOUT_H

#include <QWidget>

namespace Ui {
class LoginLayout;
}

class LoginLayout : public QWidget
{
    Q_OBJECT
public:
    explicit LoginLayout(QWidget *parent = nullptr);
    ~LoginLayout();
    inline void sendLoginSuccessSignal()
    {
        emit LoginSuccessSignal();
    }
    void messageBox(QString title,QString text);
    void logicHandler();
private:
    Ui::LoginLayout *ui;

signals:
    void LoginSuccessSignal();
};
#endif // LOGINLAYOUT_H
