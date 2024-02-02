#include "appmodel.h"
#include "mainwindow.h"
AppModel::AppModel(MainWindow *mainWindow, QObject *parent)
    : QObject(parent)
    , mMainWindow(mainWindow)
    , mNetworkReplyHistory(HISTORY_SIZE)
{
    connect(this,&AppModel::userNameChanged,mMainWindow,&MainWindow::userNameChangedSlot);

    connect(this,&AppModel::onlineChanged,mMainWindow,&MainWindow::onlineChangedSlot);
    connect(mMainWindow,&MainWindow::changeOnline,this,&AppModel::setOnline);
}

bool AppModel::readFromFile(const QString &fileName)
{
    QFile reader(fileName);
    reader.open(QIODevice::ReadOnly);
    if(!reader.isOpen()){
        //配置文件不存在
        return false;
    }
    QJsonDocument clientJsonDoc = QJsonDocument::fromJson(reader.readAll());
    reader.close();
    if(!clientJsonDoc.isObject()){
        //解析JSON失败
        return false;
    }
    QJsonObject clientJsonObject = clientJsonDoc.object();
    //读入登录模式
    setLoginMode(clientJsonObject["loginMode"].toString());
    //读入账号密码
    setAccountAndNotify(clientJsonObject["account"].toObject()["account"].toString(),
            clientJsonObject["account"].toObject()["password"].toString());
    //读入Cookies
    mCookieJar.readFromJsonArray(clientJsonObject["cookies"].toArray());

    //读入CoreData
    mCoreData.readFromFile("coredata.json");
    //TODO  : return false?

    setOnlineAndNotify(clientJsonObject["isOnline"].toBool());

    //read history
    QFile replyHistoryReader("getHistory.json");
    if(!replyHistoryReader.open(QIODevice::ReadOnly)) {
        return false;
    }
    QJsonArray replyHistoryJsonArray = QJsonDocument::fromJson(replyHistoryReader.readAll()).array();
    replyHistoryReader.close();
    for(auto it = replyHistoryJsonArray.begin();it != replyHistoryJsonArray.end();++it) {
        //TODO : 优化
        SimpleNetworkReply reply;
        reply.readFromJsonObject((*it).toObject());
        mNetworkReplyHistory.push(reply);
    }

    return true;
}

bool AppModel::writeToFile(const QString &fileName) const
{
    QFile writer(fileName);
    writer.open(QIODevice::WriteOnly);
    QJsonObject clientJsonObject;
    clientJsonObject.insert("loginMode",mLoginMode);
    clientJsonObject.insert("account",mAccount.toJsonObject());
    clientJsonObject.insert("cookies",mCookieJar.toJsonArray());
    clientJsonObject.insert("isOnline",mOnline);

    writer.write(QJsonDocument(clientJsonObject).toJson());
    writer.close();
    mCoreData.writeToFile("coredata.json");

    //write history
    QJsonArray replyHistoryJsonArray;
    for(auto it = mNetworkReplyHistory.begin();it != mNetworkReplyHistory.end();++it) {
        replyHistoryJsonArray.append((*it).toJsonObject());
    }
    QFile replyHistoryWriter("getHistory.json");
    replyHistoryWriter.open(QIODevice::WriteOnly);
    replyHistoryWriter.write(QJsonDocument(replyHistoryJsonArray).toJson());
    replyHistoryWriter.close();

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

const QString &AppModel::cacheDirectory() const
{
    return mCacheDirectory;
}

bool AppModel::isOnline() const
{
    return mOnline;
}

CoreData *AppModel::coreData()
{
    return &mCoreData;
}

const QString &AppModel::loginMode() const
{
    return mLoginMode;
}

void AppModel::setAccountAndNotify(const Account &account)
{
    mAccount = account;
    emit userNameChanged(mAccount.account);
}

void AppModel::setAccountAndNotify(const QString &arg_account, const QString &arg_password)
{
    mAccount.account = arg_account;
    mAccount.password = arg_password;
    emit userNameChanged(mAccount.account);
}

void AppModel::setCacheDirectory(const QString& cacheDirectory)
{
    if(!QDir(cacheDirectory).exists()){
        QDir::current().mkdir(cacheDirectory);
    }
    mCacheDirectory = cacheDirectory;
}

void AppModel::setOnline(bool isOnline)
{
    mOnline = isOnline;
}

void AppModel::setOnlineAndNotify(bool isOnline)
{
    mOnline = isOnline;
    emit onlineChanged(isOnline);
}

void AppModel::setLoginMode(const QString &loginMode)
{
    mLoginMode = loginMode;
}

void AppModel::clearData()
{
    setAccountAndNotify("","");
    mCookieJar.clear();
}

void AppModel::addNetworkReplyHistory(const QString &requestApi, const QByteArray &replyData)
{
    //TODO: 优化性能
    mNetworkReplyHistory.push(SimpleNetworkReply(requestApi,replyData));
}

SimpleNetworkReply *AppModel::findHistoryReply(const QString &requestApi)
{
    for(auto it = mNetworkReplyHistory.begin();it != mNetworkReplyHistory.end();++it) {
        if(requestApi == (*it).requestApi) {
            return &(*it);
        }
    }
    return nullptr;
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
