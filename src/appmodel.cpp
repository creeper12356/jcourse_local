#include "appmodel.h"
#include "mainwindow.h"
AppModel::AppModel(MainWindow *mainWindow, QObject *parent)
    : QObject(parent)
    , mMainWindow(mainWindow)
{
    connect(this,&AppModel::cacheChanged,mMainWindow,&MainWindow::cacheChangedSlot);
}

bool AppModel::readFromFile(const QString &fileName)
{
    QFile reader(fileName);
    reader.open(QIODevice::ReadOnly);
    if(!reader.isOpen()){
        return false;
    }
    //TODO : parse failed ?
    QJsonDocument clientJsonDoc = QJsonDocument::fromJson(reader.readAll());
    reader.close();
    if(!clientJsonDoc.isObject()){
        //解析JSON失败
        return false;
    }
    QJsonObject clientJsonObject = clientJsonDoc.object();
    //读入账号密码
    mAccount.account = clientJsonObject["account"].toObject()["account"].toString();
    mAccount.password = clientJsonObject["account"].toObject()["password"].toString();
    //读入Cookies
    mCookieJar.readFromJsonArray(clientJsonObject["cookies"].toArray());
    return true;
}

bool AppModel::writeToFile(const QString &fileName)
{
    QFile writer(fileName);
    writer.open(QIODevice::WriteOnly);
    QJsonObject clientJsonObject;
    clientJsonObject.insert("account",mAccount.toJsonObject());
    clientJsonObject.insert("cookies",mCookieJar.toJsonArray());
    writer.write(QJsonDocument(clientJsonObject).toJson());
    writer.close();
    return true;
}

const Account &AppModel::account() const
{
    return mAccount;
}

const MyNetworkCookieJar &AppModel::cookieJar() const
{
    return mCookieJar;
}

MyNetworkCookieJar *AppModel::cookieJarPtr()
{
    return &mCookieJar;
}

void AppModel::setAccount(const Account &account)
{
    mAccount = account;
}

void AppModel::setAccount(const QString &arg_account, const QString &arg_password)
{
    mAccount.account = arg_account;
    mAccount.password = arg_password;
}

void AppModel::setCache(const QString &cache)
{
    mCache = cache;
    emit cacheChanged(mCache);
}

QJsonObject Account::toJsonObject() const
{
    QJsonObject obj;
    obj.insert("account",account);
    obj.insert("password",password);
    return obj;
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
