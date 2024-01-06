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
    connect(mLoginWindow,&LoginWindow::rejected,mApp,&QApplication::quit);
}

bool Client::initialize()
{
    mManager->setCookieJar(&mCookieJar);
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
        mCookieJar.readFromJsonArray(clientJsonObject["cookies"].toArray());
    }
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
    QFile writer("./client.json");
    writer.open(QIODevice::WriteOnly);
    QJsonObject clientJsonObject;
    clientJsonObject.insert("account",mAccount.toJsonObject());
    clientJsonObject.insert("cookies",mCookieJar.toJsonArray());
    writer.write(QJsonDocument(clientJsonObject).toJson());
    writer.close();
}

bool Client::search(const QString &query)
{
    auto reply = getWithCookies(SEARCH_URL(query));
    //TODO : analyze reply
    qDebug() << QString::fromUtf8(reply->readAll());

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
                mAccount.account = mLoginWindow->getAccount();
                mAccount.password = mLoginWindow->getPassword();
                qDebug() << "登录账号： " << mAccount.account;
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
    mCookieJar.clear();

    //更新Cookies
    QNetworkRequest loginRequest(LOGIN_URL);
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;
    postData.addQueryItem("account",mAccount.account);
    postData.addQueryItem("password",mAccount.password);
    qDebug() << "account: " << mAccount.account;
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

MyNetworkCookieJar::MyNetworkCookieJar(QObject *parent)
    :QNetworkCookieJar(parent)
{

}

QList<QNetworkCookie> MyNetworkCookieJar::allCookies() const
{
    return QNetworkCookieJar::allCookies();
}

QJsonArray MyNetworkCookieJar::toJsonArray() const
{
    QJsonArray cookiesJsonArray;
    for(auto& cookie : allCookies()){
        qDebug() << "cookie: " <<cookie;
        QJsonObject cookieJsonObject;
        cookieJsonObject.insert("name",QString::fromUtf8(cookie.name()));
        cookieJsonObject.insert("value",QString::fromUtf8(cookie.value()));
        cookieJsonObject.insert("expirationDate",cookie.expirationDate().toString());
        cookieJsonObject.insert("domain",cookie.domain());
        cookieJsonObject.insert("secure",cookie.isSecure());
        cookieJsonObject.insert("path",cookie.path());
        cookieJsonObject.insert("httpOnly",cookie.isHttpOnly());
        cookiesJsonArray.append(cookieJsonObject);
    }
    return cookiesJsonArray;
}

bool MyNetworkCookieJar::readFromJsonArray(const QJsonArray &arr)
{
    //清除所有cookies
    setAllCookies({});

    for(auto it = arr.begin();it != arr.end();++it){
        //TODO : 检查json格式
        QNetworkCookie cookie;
        QJsonObject cookieJsonObject = (*it).toObject();
        cookie.setName(cookieJsonObject["name"].toString().toUtf8());
        cookie.setValue(cookieJsonObject["value"].toString().toUtf8());
        cookie.setExpirationDate(QDateTime::fromString(cookieJsonObject["expirationDate"].toString()));
        cookie.setDomain(cookieJsonObject["domain"].toString());
        cookie.setSecure(cookieJsonObject["secure"].toBool());
        cookie.setPath(cookieJsonObject["path"].toString());
        cookie.setHttpOnly(cookieJsonObject["httpOnly"].toBool());

        qDebug() << "cookie: " << cookie;
        insertCookie(cookie);
    }
    return true;
}

void MyNetworkCookieJar::clear()
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
