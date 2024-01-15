#ifndef CLIENT_H
#define CLIENT_H
#include "inc.h"
//客户端运行模式 ：
//在线模式： 所有资源从api获得，同时缓存入本地文件
//离线模式： 所有资源从缓存文件获得
//两种模式共享相同的接口

class Client : public QObject
{
    //客户端类
    Q_OBJECT
public:
    explicit Client(QApplication* app);
    ~Client();
public:
    //切换至主窗口
    void switchMainWindow();
    //切换至登录窗口
    void switchLoginWindow();

public slots:
    //登出
    void logout();

    //发起内容为query的检索，检索页面page
    bool search(const QString& query, int page = 1);
    //查看课号为courseid的评价，检索页面page
    bool checkReview(int courseid, int page = 1);

public:
    //向服务器发起带有cookies的get请求，
    //请求成功：返回服务器请求，返回指针指向的内存需要手动释放
    //请求失败：返回nullptr，发出login()信号
    QNetworkReply *getWithCookies(const QUrl &apiUrl);

signals:
    //向MainWindow发送搜索结果,使用JSON格式
    void searchFinished(QByteArray result);
    //向MainWindow发送查看选课评价结果，使用JSON格式
    void checkReviewFinished(QByteArray result);


private:
    //根据当前账号密码，自动更新Cookies，返回是否验证成功
    bool autoUpdateCookies();

private:
    LoginWindow* mLoginWindow = nullptr;
    MainWindow* mMainWindow = nullptr;
private:
    QApplication *mApp = nullptr;
    QEventLoop *mEventLoop = nullptr;
    QNetworkAccessManager* mManager = nullptr;
    AppModel* mAppModel = nullptr;

};

#endif // CLIENT_H
