#ifndef CLIENT_H
#define CLIENT_H
#include "inc.h"
class Account
{
public:
    QString account = "";
    QString password = "";

    Account() {}
    QJsonObject toJsonObject() const;
};
class MyNetworkCookieJar : public QNetworkCookieJar
{
public:
    MyNetworkCookieJar(QObject *parent = 0);

    QList<QNetworkCookie> allCookies() const;
    QJsonArray toJsonArray() const;
    bool readFromJsonArray(const QJsonArray &arr);
    void clear();
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
    QNetworkReply *getWithCookies(const QUrl &apiUrl);
    void updateCookies();
private:
    //(*now useless*)从服务器回应的raw cookies中获取对应键的值
    QByteArray rawCookiesValueAt(const QByteArray& rawCookies,const QString& key);

private:
    Account mAccount;
    MyNetworkCookieJar mCookies;
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
