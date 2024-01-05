#include "client.h"
#include "mainwindow.h"
#include "loginwindow.h"

Client::Client(QApplication *app)
    : QObject(nullptr)
    , mApp(app)
{
    mEventLoop = new QEventLoop(this);
    mManager = new QNetworkAccessManager(this);

    mLoginWindow = new LoginWindow(nullptr);
    mMainWindow = new MainWindow(nullptr);
    connect(mMainWindow,&MainWindow::search,this,[this](QString query){
       this->search(query);
    });

    connect(mManager,&QNetworkAccessManager::finished,mEventLoop,&QEventLoop::quit);
}

bool Client::initialize()
{
    mLoginWindow->show();
    //直到登录成功跳出循环
    while(mLoginStatus != 200){
        if(!mLoginWindow->login()){
            //用户取消登录
            mLoginStatus = -1;
            mLoginWindow->close();
            qDebug() << "break";
            break;
        }
        //获取账号密码
        mAccount.account = mLoginWindow->getAccount();
        mAccount.password = mLoginWindow->getPassword();

        qDebug() << "登录账号： " << mAccount.account;
        //发送请求获得token
        QNetworkRequest request(LOGIN_URL);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QUrlQuery postData;
        postData.addQueryItem("account",mAccount.account);
        postData.addQueryItem("password",mAccount.password);
        QNetworkReply* reply = mManager->post(request,postData.toString(QUrl::FullyEncoded).toUtf8());
        mEventLoop->exec();
        mLoginStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        mLoginWindow->displayStatusCode(mLoginStatus);
        if(reply->error() == QNetworkReply::NoError){
            mLoginWindow->accept();
            break;
        }
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        //解析cookies
        parseCookies(reply->rawHeader("set-cookie"));
        mManager->setCookieJar(&mCookies);
        reply->deleteLater();
    }
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
}

bool Client::search(const QString &query)
{
    QNetworkRequest request(SEARCH_URL(query));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = mManager->get(request);
    mEventLoop->exec();
    qDebug() << reply->readAll();
    reply->deleteLater();
    return true;
}

void Client::parseCookies(const QByteArray &rawCookies)
{
    mCookies.insertCookie(QNetworkCookie("csrftoken",rawCookiesValueAt(rawCookies,"csrftoken")));
    mCookies.insertCookie(QNetworkCookie("sessionid",rawCookiesValueAt(rawCookies,"sessionid")));
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
