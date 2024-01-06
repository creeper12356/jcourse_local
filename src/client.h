#ifndef CLIENT_H
#define CLIENT_H
#include "inc.h"
class Account
{
    //选课社区账号
public:
    QString account = "";
    QString password = "";

    Account() {}
    QJsonObject toJsonObject() const;
};
class MyNetworkCookieJar : public QNetworkCookieJar
{
    //管理Cookies的类
public:
    MyNetworkCookieJar(QObject *parent = 0);

    //cookies列表
    QList<QNetworkCookie> allCookies() const;
    //删除所有cookies
    void clear();
    //从JSON数组读取
    bool readFromJsonArray(const QJsonArray &arr);
    //转成JSON数组
    QJsonArray toJsonArray() const;
};

class Client : public QObject
{
    //客户端类
    Q_OBJECT
public:
    explicit Client(QApplication* app);
    //初始化函数，返回是否应该继续运行
    bool initialize();
    ~Client();

    bool search(const QString& query);

    //向服务器发起带有cookies的get请求，返回服务器回应
    //返回指针指向内存需要手动释放
    QNetworkReply *getWithCookies(const QUrl &apiUrl);

    //向服务器发送带有账号密码的post请求，更新cookies
    void updateCookies();
private:
    //(*now useless*)从服务器回应的raw cookies中获取对应键的值
    QByteArray rawCookiesValueAt(const QByteArray& rawCookies,const QString& key);

private:
    Account mAccount;
    MyNetworkCookieJar mCookieJar;
private:
    LoginWindow* mLoginWindow = nullptr;
    MainWindow* mMainWindow = nullptr;
private:
    QApplication *mApp = nullptr;
    QEventLoop *mEventLoop = nullptr;
    QNetworkAccessManager* mManager = nullptr;
    int mLoginStatus = 400;
};

#endif // CLIENT_H
