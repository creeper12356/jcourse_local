#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "client.h"
LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    ui->account_edit->setEchoMode(QLineEdit::Normal);
    ui->password_edit->setEchoMode(QLineEdit::Password);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

bool LoginWindow::login()
{
    QEventLoop eventLoop;
    bool isCanceled = false;
    connect(ui->cancel_button,&QPushButton::clicked,this,[&isCanceled,&eventLoop](){
        isCanceled = true;
        eventLoop.quit();
    });
    connect(ui->login_button,&QPushButton::clicked,&eventLoop,&QEventLoop::quit);
    eventLoop.exec();
    return !isCanceled;
}

void LoginWindow::displayStatusCode(int code)
{
    if(code == 200){
        //登录成功
        ui->status_label->setStyleSheet("color:green");
        ui->status_label->setText("登录成功");
    }
    else{
        ui->status_label->setStyleSheet("color:red");
        ui->status_label->setText(QString::number(code));
    }
}

QString LoginWindow::getAccount() const
{
    return ui->account_edit->text();
}

QString LoginWindow::getPassword() const
{
    return ui->password_edit->text();
}

void LoginWindow::showEvent(QShowEvent *)
{
    ui->account_edit->setFocus();
}
