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

    LOAD_QSS(QSS_FILE);

    ui->email_login_account_edit->setEchoMode(QLineEdit::Normal);
    ui->email_login_account_edit->setMaximumHeight(30);
    ui->email_login_account_edit->setPlaceholderText("jAccount用户名");

    ui->email_login_password_edit->setEchoMode(QLineEdit::Password);
    ui->email_login_password_edit->setMaximumHeight(30);
    ui->email_login_password_edit->setPlaceholderText("选课社区密码");

    ui->email_login_edit_layout->setAlignment(Qt::AlignCenter);

    ui->email_login_cancel_button->setFocusPolicy(Qt::NoFocus);

    connect(ui->email_login_login_button,&QPushButton::clicked,this,&LoginWindow::emailPasswordLogin);
    connect(ui->email_login_cancel_button,&QPushButton::clicked,this,&LoginWindow::rejected);


    ui->email_send_account_edit->setEchoMode(QLineEdit::Normal);
    ui->email_send_account_edit->setMaximumHeight(30);
    ui->email_send_account_edit->setPlaceholderText("jAccount用户名");

    ui->email_send_code_edit->setEchoMode(QLineEdit::Normal);
    ui->email_send_code_edit->setMaximumHeight(30);
    ui->email_send_code_edit->setPlaceholderText("输入验证码");

    ui->email_send_get_code_button->setMaximumHeight(30);

    ui->email_send_edit_layout->setAlignment(Qt::AlignCenter);

    ui->email_send_cancel_button->setFocusPolicy(Qt::NoFocus);

    connect(ui->email_send_get_code_button,&QPushButton::clicked,this,&LoginWindow::sendVerificationCode);
    connect(ui->email_send_login_button,&QPushButton::clicked,this,&LoginWindow::emailCodeLogin);
    connect(ui->email_send_cancel_button,&QPushButton::clicked,this,&LoginWindow::rejected);


    ui->login_account_edit->setEchoMode(QLineEdit::Normal);
    ui->login_account_edit->setMaximumHeight(30);
    ui->login_account_edit->setPlaceholderText("选课社区用户名");

    ui->login_password_edit->setEchoMode(QLineEdit::Password);
    ui->login_password_edit->setMaximumHeight(30);
    ui->login_password_edit->setPlaceholderText("选课社区密码");

    ui->login_edit_layout->setAlignment(Qt::AlignCenter);

    ui->login_cancel_button->setFocusPolicy(Qt::NoFocus);

    connect(ui->login_login_button,&QPushButton::clicked,this,&LoginWindow::userPasswordLogin);
    connect(ui->login_cancel_button,&QPushButton::clicked,this,&LoginWindow::rejected);

}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::displayReplyStatus(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        ui->status_label->setStyleSheet("color:green");
    }
    else{
        ui->status_label->setStyleSheet("color:red");
    }
    QJsonObject replyJsonObject = QJsonDocument::fromJson(reply->readAll()).object();
    QString detail = replyJsonObject["detail"].toString();
    ui->status_label->setText(detail);
}

void LoginWindow::emailPasswordLogin()
{
    QEventLoop eventLoop;
    connect(mManager,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);

    //清除之前的Cookies
    cookieJar()->clear();

    //更新Cookies
    QNetworkRequest loginRequest(EMAIL_LOGIN_URL);
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;

    QString account = ui->email_login_account_edit->text();
    QString password = ui->email_login_password_edit->text();
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
    displayReplyStatus(reply);
    delete reply;

    if(error == QNetworkReply::NoError){
        //TODO : BUG ，连点可以强行登录
        //登录成功
        emit emailPasswordLoginSuccess(account,password);
    }
}

void LoginWindow::sendVerificationCode()
{
    QEventLoop eventLoop;
    connect(mManager,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);

    //清除之前的Cookies
    cookieJar()->clear();
    QNetworkRequest sendCodeRequest(EMAIL_SEND_CODE_URL);
    sendCodeRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;

    QString account = ui->email_send_account_edit->text();
    if(account.isEmpty()){
        qDebug() << "account cannot be empty";
        return ;
    }
    postData.addQueryItem("account",account);
    qDebug() << "send code to account: " << account;
    QNetworkReply* reply = mManager->post(sendCodeRequest,postData.toString(QUrl::FullyEncoded).toUtf8());
    eventLoop.exec();
    displayReplyStatus(reply);
    delete reply;
}

void LoginWindow::emailCodeLogin()
{
    QEventLoop eventLoop;
    connect(mManager,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);

    //清除之前的Cookies
    cookieJar()->clear();

    //更新Cookies
    QNetworkRequest loginRequest(EMAIL_VERIFY_URL);
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;

    QString account = ui->email_send_account_edit->text();
    QString code = ui->email_send_code_edit->text();
    if(account.isEmpty() || code.isEmpty()){
        qDebug() << "account or code cannot be empty.";
        return ;
    }
    postData.addQueryItem("account",account);
    postData.addQueryItem("code",code);
    qDebug() << "try login: " << account;
    QNetworkReply* reply = mManager->post(loginRequest,postData.toString(QUrl::FullyEncoded).toUtf8());
    eventLoop.exec();
    auto error = reply->error();
    displayReplyStatus(reply);
    delete reply;

    if(error == QNetworkReply::NoError){
        //TODO : BUG ，连点可以强行登录
        //登录成功
        emit emailCodeLoginSuccess(account);
    }
}

void LoginWindow::userPasswordLogin()
{
    QEventLoop eventLoop;
    connect(mManager,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);

    //清除之前的Cookies
    cookieJar()->clear();

    //更新Cookies
    QNetworkRequest loginRequest(LOGIN_URL);
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;

    QString username = ui->login_account_edit->text();
    QString password = ui->login_password_edit->text();
    if(username.isEmpty() || password.isEmpty()){
        qDebug() << "username or password cannot be empty.";
        return ;
    }
    postData.addQueryItem("username",username);
    postData.addQueryItem("password",password);
    qDebug() << "try login: " << username;
    QNetworkReply* reply = mManager->post(loginRequest,postData.toString(QUrl::FullyEncoded).toUtf8());
    eventLoop.exec();
    auto error = reply->error();
    displayReplyStatus(reply);
    delete reply;

    if(error == QNetworkReply::NoError){
        //TODO : BUG ，连点可以强行登录
        //登录成功
        emit userPasswordLoginSuccess(username,password);
    }
}

void LoginWindow::showEvent(QShowEvent *)
{
    ui->email_login_account_edit->setFocus();
    //    ui->login_button->setFocus();
}

CustomNetworkCookieJar *LoginWindow::cookieJar()
{
    return dynamic_cast<CustomNetworkCookieJar*>(mManager->cookieJar());
}
