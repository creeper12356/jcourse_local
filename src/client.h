#ifndef CLIENT_H
#define CLIENT_H
#include "inc.h"
namespace client {
//客户端运行模式 ：
//在线模式： 所有资源从api获得，同时缓存入本地文件
//离线模式： 所有资源从缓存文件获得
//两种模式共享相同的接口

enum Mode {Online , Offline};
}
class Client : public QObject
{
    //客户端类
    Q_OBJECT
public:
    explicit Client(QApplication* app);
    //初始化函数，返回是否应该继续运行
    bool initialize();
    ~Client();

    //发起内容为query的检索，检索页面page
    bool search(const QString& query, int page = 1);
    //查看课号为courseid的评价，检索页面page
    bool checkReview(int courseid, int page = 1);

    //向服务器发起带有cookies的get请求，返回服务器回应
    //返回指针指向内存需要手动释放
    QNetworkReply *getWithCookies(const QUrl &apiUrl);

    //向服务器发送带有账号密码的post请求，更新cookies
    bool updateCookies();

private:
    LoginWindow* mLoginWindow = nullptr;
    MainWindow* mMainWindow = nullptr;
private:
    QApplication *mApp = nullptr;
    QEventLoop *mEventLoop = nullptr;
    QNetworkAccessManager* mManager = nullptr;
    AppModel* mAppModel = nullptr;
    //客户端模式
    client::Mode mMode = client::Online;
signals:
    //向MainWindow发送搜索结果,使用JSON格式
    void searchFinished(QByteArray result);
    //向MainWindow发送查看选课评价结果，使用JSON格式
    void checkReviewFinished(QByteArray result);
};

#endif // CLIENT_H
