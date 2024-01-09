#include "client.h"
#include "mainwindow.h"
#include "loginwindow.h"

#include "appmodel.h"

Client::Client(QApplication *app)
    : QObject(nullptr)
    , mApp(app)
{
    mEventLoop = new QEventLoop(this);
    mManager = new QNetworkAccessManager(this);
    mAppModel = new AppModel;

    mLoginWindow = new LoginWindow(nullptr);
    mMainWindow = new MainWindow(nullptr);
    connect(mMainWindow,&MainWindow::search,this,[this](QString query){
        this->search(query);
    });

    connect(mManager,&QNetworkAccessManager::finished,mEventLoop,&QEventLoop::quit);
    connect(mLoginWindow,&LoginWindow::rejected,mApp,&QApplication::quit);
}

bool Client::initialize()
{
    mManager->setCookieJar(mAppModel->cookieJarPtr());
    mAppModel->readFromFile("./client.json");
    mMainWindow->show();
    return true;
    //control never reaches here


    //登录
    //TODO : 请求失败时弹出登录窗口

    mLoginWindow->show();
    //直到登录成功跳出循环
    // from here
    while(mLoginStatus != 200){
        if(!mLoginWindow->login()){
            //用户取消登录
            mLoginStatus = -1;
            mLoginWindow->close();
            qDebug() << "break";
            break;
        }
        //获取账号密码
//        mAccount.account = mLoginWindow->getAccount();
//        mAccount.password = mLoginWindow->getPassword();

        qDebug() << "登录账号： " << mAppModel->account().toJsonObject();
        //发送请求获得token
        QNetworkRequest request(LOGIN_URL);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QUrlQuery postData;
//        postData.addQueryItem("account",mAccount.account);
//        postData.addQueryItem("password",mAccount.password);
        QNetworkReply* reply = mManager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
        mEventLoop->exec();
        mLoginStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        mLoginWindow->displayStatusCode(mLoginStatus);
        if(reply->error() == QNetworkReply::NoError){
            mLoginWindow->accept();
            break;
        }
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
//        reply->deleteLater();
    }
    //to here
    if(mLoginStatus == 200){
        //登录成功
        mMainWindow->show();
        return true;
    }
    else{
        //取消登录
        return false;
    }
}

Client::~Client()
{
    delete mLoginWindow;
    delete mMainWindow;

    //write to file
    qDebug() << "write to file";
    mAppModel->writeToFile("./client.json");

    delete mAppModel;
}

bool Client::search(const QString &query)
{
    auto reply = getWithCookies(SEARCH_URL(query));
    //TODO : analyze reply
    QJsonObject result = QJsonDocument::fromJson(reply->readAll()).object();
    qDebug() << "result : " << result;

    delete reply;
    return true;
}

QNetworkReply *Client::getWithCookies(const QUrl &apiUrl)
{
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = nullptr;
    while(true){
        reply = mManager->get(request);
        mEventLoop->exec();
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << "该cookies存在问题";
            qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            qDebug() << QString::fromUtf8(reply->readAll());
            //使用Cookies出现问题
            while(!updateCookies()){
                mMainWindow->close();
                mLoginWindow->show();

                if(!mLoginWindow->login()){
                    //用户取消登录
                    mLoginStatus = -1;
                    //关闭程序
                    mLoginWindow->reject();
                    qDebug() << "canceled";
                }
                //获取账号密码
                mAppModel->setAccount(mLoginWindow->getAccount(),mLoginWindow->getPassword());
                qDebug() << "登录账号： " << mAppModel->account().account;
            }

            mMainWindow->show();
            mLoginWindow->accept();

            delete reply;
            continue;
        }
        break;
    }
    //reply需要caller手动销毁
    return reply;
}

bool Client::updateCookies()
{
    //清除之前的Cookies
    mAppModel->cookieJarPtr()->clear();

    //更新Cookies
    QNetworkRequest loginRequest(LOGIN_URL);
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;
    postData.addQueryItem("account",mAppModel->account().account);
    postData.addQueryItem("password",mAppModel->account().password);
    qDebug() << "account: " << mAppModel->account().account;
    QNetworkReply* reply = mManager->post(loginRequest,postData.toString(QUrl::FullyEncoded).toUtf8());
    mEventLoop->exec();
    auto error = reply->error();
    delete reply;
    return error == QNetworkReply::NoError;
}


QByteArray Client::rawCookiesValueAt(const QByteArray &rawCookies, const QString &key)
{
    int valueIndex = rawCookies.indexOf(key + "=");
    if(valueIndex == -1){
        return "";
    }
    valueIndex += key.length() + 1;
    int semicolonIndex = rawCookies.indexOf(";",valueIndex);
    if(semicolonIndex == -1){
        return "";
    }
    return rawCookies.mid(valueIndex,semicolonIndex - valueIndex);
}
