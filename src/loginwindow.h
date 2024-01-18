#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include "inc.h"
namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    LoginWindow(QNetworkAccessManager* manager , QWidget *parent = nullptr);
    ~LoginWindow();

    void displayStatusCode(int code);
    //setters
    void setAccount(const QString& account);
    void setPassword(const QString& password);

    //getters
    QString getAccount() const;
    QString getPassword() const;

    //登录函数，登录成功发送loginSuccess信号
    void login();

protected:
    void showEvent(QShowEvent* );

signals:
    //用户登录成功信号，发送给Client
    void loginSuccess(QString account , QString password);

private:
    Ui::LoginWindow *ui;
    //指向Client.mManager，用于发送登录请求
    QNetworkAccessManager* mManager;

};

#endif // LOGINWINDOW_H
