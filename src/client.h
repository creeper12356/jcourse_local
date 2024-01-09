#ifndef CLIENT_H
#define CLIENT_H
#include "inc.h"

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
    bool updateCookies();

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
