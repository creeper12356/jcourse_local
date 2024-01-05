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
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    //返回true表示用户登录，返回false表示取消
    bool login();
    void displayStatusCode(int code);
    //接口
    QString getAccount() const;
    QString getPassword() const;
protected:
    void showEvent(QShowEvent* );

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
