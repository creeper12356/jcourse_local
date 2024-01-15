#ifndef APPMODEL_H
#define APPMODEL_H

#include "coredatastructure.h"
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
    //存储客户端的所有永久性数据
    Q_OBJECT
public:
    AppModel(MainWindow* mainWindow, QObject *parent = nullptr);

public:
    //从JSON文件中读取数据，返回读取结果
    bool readFromFile(const QString& fileName);
    //将数据写入JSON文件，返回写入结果
    bool writeToFile(const QString& fileName);
public:
    //getters
    const Account& account() const;
    const MyNetworkCookieJar& cookieJar () const;
    MyNetworkCookieJar* cookieJarPtr();
    const QString& cacheDirectory() const;
    bool isOnline() const;
    CoreData* coreData();

public slots:
    //setters
    //setProperty 直接设置，不发出propertyChanged信号
    //setPropertyAndNotify 设置，且发送propertyChanged信号，MainWindow接受该信号，界面作出响应
    void setAccountAndNotify(const Account& account);
    void setAccountAndNotify(const QString& account , const QString& password);
    void setCacheDirectory(const QString &cacheDirectory);
    void setOnline(bool isOnline);
    void setOnlineAndNotify(bool isOnline);

signals:
    //发送给MainWindow的信号，更新显示数据
    void userNameChanged(QString userName) ;
    void onlineChanged(bool online);
private:
    //显示界面
    MainWindow* mMainWindow = nullptr;
private:
    //选课社区邮箱账号密码
    Account mAccount;
    MyNetworkCookieJar mCookieJar;
    //是否为在线模式
    //在线模式：从url获取资源，缓存到本地
    //离线模式：从本地缓存获取资源
    bool mOnline;
    //缓存路径,默认为cache
    QString mCacheDirectory;
    CoreData mCoreData;

};

#endif // APPMODEL_H
