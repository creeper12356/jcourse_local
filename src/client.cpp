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
    //read from file
    //TODO : 封装readFromFile 方法
    //TODO : 异常处理
    QFile reader("./client.json");
    reader.open(QIODevice::ReadOnly);
    if(reader.isOpen()){
        //TODO : parse failed ?
        QJsonObject clientJsonObject = QJsonDocument::fromJson(reader.readAll()).object();
        reader.close();
        //读入账号密码
        mAccount.account = clientJsonObject["account"].toObject()["account"].toString();
        mAccount.password = clientJsonObject["account"].toObject()["password"].toString();
        //读入Cookies
        QJsonObject cookiesJsonObject = clientJsonObject["cookies"].toObject();
        qDebug() << "after read from file: ";
        qDebug() << mAccount.toJsonObject();
        for(auto it = cookiesJsonObject.begin();it != cookiesJsonObject.end() ; ++it){
            //assert mCookies is empty
            mCookies.insertCookie(QNetworkCookie(it.key().toUtf8(),it.value().toString().toUtf8()));
        }
    }
    //只需要在初始化时设置一次
    mManager->setCookieJar(&mCookies);
    mMainWindow->show();
    return true;
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
    QFile writer("./client.json");
    writer.open(QIODevice::WriteOnly);
    QJsonObject clientJsonObject;
    clientJsonObject.insert("account",mAccount.toJsonObject());
    clientJsonObject.insert("cookies",mCookies.toJsonObject());
    writer.write(QJsonDocument(clientJsonObject).toJson());
    writer.close();
}

bool Client::search(const QString &query)
{
    auto reply = getWithCookies(SEARCH_URL(query));
    //TODO : analyze reply
    qDebug() << QString::fromUtf8(reply->readAll());

    reply->deleteLater();
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
            updateCookies();
//            reply->deleteLater();
            continue;
        }
        break;
    }
    //reply需要caller手动销毁
    return reply;
}

void Client::updateCookies()
{
    QNetworkRequest loginRequest(LOGIN_URL);
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;
    postData.addQueryItem("account",mAccount.account);
    postData.addQueryItem("password",mAccount.password);
    QNetworkReply* reply = mManager->post(loginRequest,postData.toString(QUrl::FullyEncoded).toUtf8());
    mEventLoop->exec();
    if(reply->error() != QNetworkReply::NoError){
        reply->deleteLater();
        qDebug() << "throw " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        throw reply->error();
    }
    //请求获得Cookies成功
    //说明之前的Cookies过期
        //TODO : 封装parseCookies
    //解析并更新cookies
    qDebug() << "new cookies updated.";
    parseCookies(reply->rawHeader("set-cookie"));
    qDebug() << mCookies.toJsonObject();
    reply->deleteLater();
}


void Client::parseCookies(const QByteArray &rawCookies)
{
    mCookies.deleteAllCookies();
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

MyNetWorkCookieJar::MyNetWorkCookieJar(QObject *parent)
    :QNetworkCookieJar(parent)
{

}

QJsonObject MyNetWorkCookieJar::toJsonObject() const
{
    QJsonObject obj;
    for(auto cookie : allCookies()){
        obj.insert(QString::fromUtf8(cookie.name()),QString::fromUtf8(cookie.value()));
    }
    return obj;
}

void MyNetWorkCookieJar::deleteAllCookies()
{
    setAllCookies({});
}


QJsonObject Account::toJsonObject() const
{
    QJsonObject obj;
    obj.insert("account",account);
    obj.insert("password",password);
    return obj;
}
