#ifndef APPMODEL_H
#define APPMODEL_H

#include "coredatastructure.h"
#include "inc.h"
#include "networkreplyhistory.h"

/*!
 * \brief 选课社区账号
 */
class Account
{
public:
    QString account = "";
    QString password = "";

    Account() {}
    QJsonObject toJsonObject() const;
};

/*!
 * \brief 管理Cookies的类
 */
class CustomNetworkCookieJar : public QNetworkCookieJar
{
public:
    CustomNetworkCookieJar(QObject *parent = 0);

    /*!
     * \brief 所有Cookies的列表
     * \return 所有Cookies的列表
     */
    QList<QNetworkCookie> allCookies() const;

    /*!
     * \brief 清除所有的Cookies
     */
    void clear();

    /*!
     * \brief 从QJsonArray对象中读取信息
     * \param arr 读取的QJsonArray对象
     * \return 是否读取成功
     */
    bool readFromJsonArray(const QJsonArray &arr);

    /*!
     * \brief 转换为QJsonArray对象
     */
    QJsonArray toJsonArray() const;
};

/*!
 * \brief 存储客户端的所有永久性数据
 */
class AppModel : public QObject
{
    Q_OBJECT
public:
    AppModel(MainWindow* mainWindow, QObject *parent = nullptr);

public:
    /*!
     * \brief 从文件中读取数据
     * \param fileName 读取的文件路径
     * \return 读取是否成功
     */
    bool readFromFile(const QString& fileName);

    /*!
     * \brief 将当前信息写入文件
     * \param fileName 写入文件的路径
     */
    bool writeToFile(const QString& fileName) const;
public:
    const QString& loginMode() const;		     //!<登录模式
    const Account& account() const;                 //!<当前账号（包含账号和密码)
    const CustomNetworkCookieJar& cookieJar () const;   //!<当前cookieJar，不可修改
    CustomNetworkCookieJar* cookieJarPtr();             //!<指向当前cookieJar的指针，可修改
    const QString& cacheDirectory() const;          //!<课程评价缓存目录
    bool isOnline() const;                          //!<是否为在线模式
    CoreData* coreData();                           //!<指向核心数据的指针
    NetworkReplyHistory* networkReplyHistory();     //!<指向请求历史的指针

public slots:
    void setLoginModeAndNotify(const QString& loginMode);	                         //!<设置登录模式
    void setAccountAndNotify(const Account& account);                           //!<设置账号并通知MainWindow
    void setAccountAndNotify(const QString& account , const QString& password); //!<设置账号并通知MainWindow
    void setOnline(bool isOnline);                                              //!<设置在线模式，不通知MainWindow
    void setOnlineAndNotify(bool isOnline);                                     //!<设置在线模式并通知MainWindow

    /*!
     * \brief 清除所有数据，包括账号/密码，Cookies
     */
    void clearData();

signals:

    /*!
     * \brief 账号用户名变化的信号，发送给MainWindow
     * \param userName 当前账号用户名
     */
    void userNameChanged(QString userName) ;

    /*!
     * \brief 在线模式变化的信号，发送给MainWindow
     * \param online 是否为在线模式
     */
    void onlineChanged(bool online);

    /*!
     * \brief 登录模式变化信号
     * \param loginMode 登录模式
     */
    void loginModeChanged(QString loginMode);
private:
    //显示界面
    MainWindow* mMainWindow = nullptr;
private:
    //登录模式：emailPasswordLogin,emailCodeLogin,userPasswordLogin三种模式
    QString mLoginMode;
    //选课社区邮箱账号密码
    Account mAccount;
    CustomNetworkCookieJar mCookieJar;
    //是否为在线模式
    //在线模式：从url获取资源，缓存到本地
    //离线模式：从本地缓存获取资源
    bool mOnline;
    //核心数据
    CoreData mCoreData;
    //请求记录
    NetworkReplyHistory mNetworkReplyHistory;

};

#endif // APPMODEL_H
