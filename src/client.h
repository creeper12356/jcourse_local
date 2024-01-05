#ifndef CLIENT_H
#define CLIENT_H
#include "inc.h"
class Account
{
public:
    QString account = "";
    QString password = "";

    Account() {}
};

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QApplication* app);
    //初始化函数，返回是否应该继续运行
    bool initialize();
    ~Client();
    bool search(const QString& query);
private:
    //解析raw cookies 并更新mCookies字段
    void parseCookies(const QByteArray &rawCookies);
    //从服务器回应的raw cookies中获取对应键的值
    QByteArray rawCookiesValueAt(const QByteArray& rawCookies,const QString& key);

private:
    Account mAccount;
    QNetworkCookieJar mCookies;
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
