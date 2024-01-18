#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "client.h"
#include "appmodel.h"

LoginWindow::LoginWindow(QNetworkAccessManager *manager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow),
    mManager(manager)
{
    ui->setupUi(this);
    ui->account_edit->setEchoMode(QLineEdit::Normal);
    ui->password_edit->setEchoMode(QLineEdit::Password);

    connect(ui->login_button,&QPushButton::clicked,this,&LoginWindow::login);
    connect(ui->cancel_button,&QPushButton::clicked,this,&LoginWindow::rejected);
}

LoginWindow::~LoginWindow()
{
    delete ui;
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

void LoginWindow::setAccount(const QString &account)
{
    ui->account_edit->setText(account);
}

void LoginWindow::setPassword(const QString &password)
{
    ui->password_edit->setText(password);
}

QString LoginWindow::getAccount() const
{
    return ui->account_edit->text();
}

QString LoginWindow::getPassword() const
{
    return ui->password_edit->text();
}

void LoginWindow::login()
{
    MyNetworkCookieJar* cookieJar = dynamic_cast<MyNetworkCookieJar*> (mManager->cookieJar());
    QEventLoop eventLoop;
    connect(mManager,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);

    //清除之前的Cookies
    cookieJar->clear();

    //更新Cookies
    QNetworkRequest loginRequest(LOGIN_URL);
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;
    //TODO : assert not empty
    QString account = ui->account_edit->text();
    QString password = ui->password_edit->text();
    if(account.isEmpty() || password.isEmpty()){
        qDebug() << "account or password cannot be empty.";
        return ;
    }
    postData.addQueryItem("account",account);
    postData.addQueryItem("password",password);
    qDebug() << "try login: " << account;
    QNetworkReply* reply = mManager->post(loginRequest,postData.toString(QUrl::FullyEncoded).toUtf8());
    eventLoop.exec();
    auto error = reply->error();
    displayStatusCode(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    delete reply;

    if(error == QNetworkReply::NoError){
        //TODO : BUG ，连点可以强行登录
        //登录成功
        emit loginSuccess(account,password);
    }
}
void LoginWindow::showEvent(QShowEvent *)
{
    ui->account_edit->setFocus();
}
