#ifndef APPMODEL_H
#define APPMODEL_H

#include <QObject>
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

class AppModel : public QObject
{
    //存储客户端数据的类
    Q_OBJECT
public:
    AppModel(MainWindow* mainWindow, QObject *parent = nullptr);

public:
    //从JSON文件中读取数据，返回读取结果
    bool readFromFile(const QString& fileName);
    //将数据写入JSON文件，返回写入结果
    bool writeToFile(const QString& fileName);
public:
    const Account& account() const;
    const MyNetworkCookieJar& cookieJar () const;
    MyNetworkCookieJar* cookieJarPtr();

    void setAccount(const Account& account);
    void setAccount(const QString& account , const QString& password);
    void setCache(const QString& cache);

signals:
    void cacheChanged(QString cache) const;
    void userNameChanged(QString userName) const;

private:
    //显示界面
    MainWindow* mMainWindow = nullptr;
private:
    Account mAccount;
    MyNetworkCookieJar mCookieJar;
    //test
    QString mCache;


};

#endif // APPMODEL_H
