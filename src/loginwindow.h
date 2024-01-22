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

    /*!
     * \brief 登录界面显示服务器回应
     * \param reply 服务器回应指针
     */
    void displayReplyStatus(QNetworkReply* reply);

public slots:
    /*!
     * \brief 邮箱密码登录
     *
     * 通过邮箱密码登录，登录成功后发送emailPasswordLoginSuccess信号
     * \sa emailPasswordLoginSuccess
     */
    void emailPasswordLogin();

    /*!
     * \brief 发送邮箱验证码
     */
    void sendVerificationCode();

    /*!
     * \brief 邮箱验证登录
     *
     * 通过邮箱验证登录，登录成功后发送emailCodeLoginSuccess信号
     * \sa emailCodeLoginSuccess
     */
    void emailCodeLogin();


    /*!
     * \brief 账号密码登录
     *
     * 通过账号密码登录，登录成功后发送userPasswordLoginSuccess信号
     * \sa userPasswordLoginSuccess
     */
    void userPasswordLogin();

protected:
    void showEvent(QShowEvent* );

signals:

    /*!
     * \brief 邮箱密码登录成功信号
     *
     * 用户通过邮箱密码登录成功后，此信号被发送给Client。
     * \param account 用户名
     * \param password 密码
     * \sa emailPasswordLogin
     */
    void emailPasswordLoginSuccess(QString account , QString password);

    /*!
     * \brief 邮箱验证登录成功信号
     *
     * 用户通过邮箱验证登录成功后，此信号被发送给Client。
     * \param account 用户名
     * \sa emailCodeLogin
     */
    void emailCodeLoginSuccess(QString account);

    void userPasswordLoginSuccess(QString user, QString password);
protected:
    MyNetworkCookieJar* cookieJar();
private:
    Ui::LoginWindow *ui;
    QNetworkAccessManager* mManager;
};

#endif // LOGINWINDOW_H
