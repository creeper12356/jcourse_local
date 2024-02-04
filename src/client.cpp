#include "client.h"
#include "mainwindow.h"
#include "loginwindow.h"
#include "pinyin.h"

#include "appmodel.h"
#include "paginationwidget.h"

Client::Client(QApplication *app)
    : QObject(nullptr)
    , mApp(app)
{
    mEventLoop = new QEventLoop(this);
    mManager = new QNetworkAccessManager(this);

    mLoginWindow = new LoginWindow(mManager , nullptr);
    mMainWindow = new MainWindow(nullptr);

    mAppModel = new AppModel(mMainWindow);

    //连接登录成功信号的槽
    connect(mLoginWindow,&LoginWindow::emailPasswordLoginSuccess,this,&Client::emailPasswordLoginSuccess);
    connect(mLoginWindow,&LoginWindow::emailCodeLoginSuccess,this,&Client::emailCodeLoginSuccess);
    connect(mLoginWindow,&LoginWindow::userPasswordLoginSuccess,this,&Client::userPasswordLoginSuccess);
    connect(mLoginWindow,&LoginWindow::offlineLoginSuccess,this,&Client::offlineLoginSuccess);


    connect(mMainWindow,&MainWindow::search,this,&Client::search);
    connect(this,&Client::searchFinished,mMainWindow,&MainWindow::displaySearchResult);

    connect(mMainWindow,&MainWindow::checkReview,this,&Client::checkReview);
    connect(this,&Client::checkReviewFinished,mMainWindow,&MainWindow::displayCheckReviewResult);

    connect(mMainWindow,&MainWindow::logout,this,&Client::logout);

    connect(mManager,&QNetworkAccessManager::finished,mEventLoop,&QEventLoop::quit);
    connect(mLoginWindow,&LoginWindow::rejected,mApp,&QApplication::quit);

    mManager->setCookieJar(mAppModel->cookieJarPtr());
    mAppModel->readFromFile("./client.json");
    mAppModel->setCacheDirectory("cache");

    connect(mMainWindow,&MainWindow::parseCourseStatus,this,&Client::parseCourseStatus);
    connect(this,&Client::parseCourseStatusFinished,mMainWindow,&MainWindow::displayParseCourseStatusResult);

    connect(mMainWindow,&MainWindow::cacheCourseReview,this,&Client::cacheCourseReview);
    connect(mMainWindow,&MainWindow::cacheCourseCodeReview,this,&Client::cacheCourseCodeReview);

    if(mAppModel->account().account.isEmpty()){
        //用户名为空，说明未登录
        switchLoginWindow();
    }
    else{
        //已经登录
        switchMainWindow();
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

void Client::switchMainWindow()
{
    mMainWindow->show();
    mLoginWindow->accept();
}

void Client::switchLoginWindow()
{
    mLoginWindow->show();
    mMainWindow->hide();
}

void Client::emailPasswordLoginSuccess(QString account, QString password)
{
    mAppModel->setLoginModeAndNotify("emailPasswordLogin");
    mAppModel->setOnlineAndNotify(true);
    mAppModel->setAccountAndNotify(account,password);
    switchMainWindow();
}

void Client::emailCodeLoginSuccess(QString account)
{
    mAppModel->setLoginModeAndNotify("emailCodeLogin");
    mAppModel->setOnlineAndNotify(true);
    mAppModel->setAccountAndNotify(account,"");
    switchMainWindow();

}

void Client::userPasswordLoginSuccess(QString user, QString password)
{
    mAppModel->setLoginModeAndNotify("userPasswordLogin");
    mAppModel->setOnlineAndNotify(true);
    mAppModel->setAccountAndNotify(user,password);
    switchMainWindow();
}

void Client::offlineLoginSuccess()
{
    mAppModel->setLoginModeAndNotify("offlineLogin");
    mAppModel->setAccountAndNotify("guest","");
    switchMainWindow();
}

void Client::warning(const QString &msg)
{
    QMessageBox::warning(mMainWindow,"警告",msg);
}

bool Client::search(const QString &query,int page)
{
    QByteArray replyData;
    if(mAppModel->isOnline()){
        //在线模式
        //请求api,缓存资源到本地
        replyData = getSearchResult(query,page);
        if(replyData.isEmpty()){
            return false;
        }

        //缓存数据
        cacheSearchResult(replyData);
    }
    else{
        //离线模式
        //本地资源搜索结果
        QVector<const Mapping*> courseMappings =
                mAppModel->coreData()->searchCourseMappings(query,query,query,query,query);
        QJsonObject resultJsonObject;
        QJsonArray resultJsonArray;

        int searchCount = courseMappings.count();
        resultJsonObject.insert("count",searchCount);
        //搜索的课程条目下标上下界
        //上界不一定能取到
        const int lowerBound = (page - 1) * PAGE_SIZE , higherBound = page * PAGE_SIZE - 1;
        for(int i = lowerBound;i < searchCount && i <= higherBound;++i){

            const Mapping* mapping = courseMappings[i];
            QJsonObject courseJsonObject;

            courseJsonObject.insert("id",mapping->course->id);
            courseJsonObject.insert("code",mapping->course->code);
            courseJsonObject.insert("name",mapping->course->name);
            courseJsonObject.insert("teacher",mapping->teacher->name);
            courseJsonObject.insert("credit",mapping->course->credit);
            courseJsonObject.insert("department",mapping->course->department);
            courseJsonObject.insert("rating",
                                    QJsonObject({{"avg",mapping->course->ratingAvg},
                                                 {"count",mapping->course->ratingCount}}));

            resultJsonArray.push_back(courseJsonObject);
        }
        resultJsonObject.insert("results",resultJsonArray);
        replyData = QJsonDocument(resultJsonObject).toJson();
    }

    emit searchFinished(replyData);
    return true;
}

bool Client::checkReview(int courseid, int page)
{
    QByteArray replyData;
    QString cacheReviewFileName = mAppModel->cacheDirectory() + "/" + CACHE_REVIEW_BASENAME(courseid,page);
    qDebug() << "cacheReviewFileName: "  << cacheReviewFileName;
    if(mAppModel->isOnline()){

        replyData = getCourseReview(courseid,page);
        if(replyData.isEmpty()){
            return false;
        }

        //缓存资源
        cacheReplyData(replyData,cacheReviewFileName);
    }
    else{
        QFile loader;
        loader.setFileName(cacheReviewFileName);
        loader.open(QIODevice::ReadOnly);
        if(loader.isOpen()){
            qDebug() << "read from cached file: " << loader.fileName();
            replyData = loader.readAll();
            loader.close();
        }
        else{
            qDebug() << "no cache can be read";
            //空JSON
            replyData = "{\"count\":0}";
        }
    }

    emit checkReviewFinished(replyData);
    return true;
}

void Client::parseCourseStatus(QString src)
{
    QTextStream ts(&src);
    QJsonObject resultJsonObject;
    QJsonArray resultJsonArray;
    while(!ts.atEnd()){
        QString line = ts.readLine();
        QStringList splitedStrList = line.split("\t",QString::SkipEmptyParts);
        QJsonObject courseCodeObject;
        if(splitedStrList.size() == 8){
//            qDebug() << splitedStrList[0];
            if(splitedStrList[0].trimmed() == "姓名" || splitedStrList[0].trimmed() == "学号" || splitedStrList[0].trimmed() == "学院"){
                continue;
            }
            courseCodeObject.insert("code",splitedStrList[0].trimmed());
            courseCodeObject.insert("name",splitedStrList[1].trimmed());
            courseCodeObject.insert("semester",splitedStrList[2].trimmed() + "-" + splitedStrList[3].trimmed());
            courseCodeObject.insert("credit",splitedStrList[4].trimmed());
            resultJsonArray.append(courseCodeObject);
        }
    }

    resultJsonObject.insert("results",resultJsonArray);
//    qDebug() << resultJsonObject;
    emit parseCourseStatusFinished(resultJsonObject);
}

QByteArray Client::getSearchResult(const QString &query, int page)
{
    return getApiData(SEARCH_URL(query,page));
}

QByteArray Client::getCourseReview(int courseid, int page)
{
    return getApiData(REVIEW_URL(courseid,page));
}

void Client::cacheCourseReview(int courseid)
{
    if(!mAppModel->isOnline()){
        warning("离线模式下，无法下载课程评价。");
        return ;
    }
    assert(mAppModel->isOnline());
    QByteArray replyData;
    int pageCount , pageCurrent = 1;

    do {
        replyData = getCourseReview(courseid,pageCurrent);
        if(replyData.isEmpty()){
            qDebug() << "error";
            return ;
        }
        if(pageCurrent == 1){
            pageCount = PaginationWidget::divideTotal(QJsonDocument::fromJson(replyData).object()["count"].toInt(),PAGE_SIZE);
        }

        cacheReplyData(replyData , mAppModel->cacheDirectory() + "/" + CACHE_REVIEW_BASENAME(courseid,pageCurrent));
        ++pageCurrent;
    } while(pageCurrent <= pageCount);

    qDebug() << "cache course " << courseid << " finished!";
}

void Client::cacheCourseCodeReview(QString courseCode)
{
    if(!mAppModel->isOnline()){
        warning("离线模式下，无法下载课程评价。");
        return ;
    }
    assert(mAppModel->isOnline());
    QByteArray replyData;
    int pageCount , pageCurrent = 1;
    QVector<int> courseidList = {};

    do {
        replyData = getSearchResult(courseCode,pageCurrent);
        if(replyData.isEmpty()){
            qDebug() << "error";
            return ;
        }
        auto replyJsonObject = QJsonDocument::fromJson(replyData).object();
        if(pageCurrent == 1){
            pageCount = PaginationWidget::divideTotal(replyJsonObject["count"].toInt(),PAGE_SIZE);
        }
        QJsonArray resultJsonArray = replyJsonObject["results"].toArray();
        for(auto it = resultJsonArray.begin();it != resultJsonArray.end();++it){
            courseidList.push_back((*it).toObject()["id"].toInt());
        }

        cacheSearchResult(replyJsonObject);
        ++pageCurrent;
    } while(pageCurrent <= pageCount);

    for(int courseid: courseidList){
        cacheCourseReview(courseid);
    }
    qDebug() << "cache course code " << courseCode << " finished!";
}

void Client::cacheReplyData(const QByteArray &replyData, const QString &fileName)
{
    QFile downloader;
    downloader.setFileName(fileName);
    qDebug() << "cache to file: " << downloader.fileName();
    downloader.open(QIODevice::WriteOnly);
    downloader.write(replyData);
    downloader.close();
}

void Client::cacheSearchResult(const QByteArray &replyData)
{
    QJsonObject replyJsonObject = QJsonDocument::fromJson(replyData).object();
    cacheSearchResult(replyJsonObject);
}

void Client::cacheSearchResult(const QJsonObject &replyJsonObject)
{
    auto resultsJsonArray = replyJsonObject["results"].toArray();
    bool ok;
    for(auto it = resultsJsonArray.begin();it != resultsJsonArray.end(); ++it){

        Course *newCourse = mAppModel->coreData()->addCourse((*it).toObject()["id"].toInt(),(*it).toObject()["name"].toString());
        //无论课程之前是否存在，更新课程数据
        newCourse->code = (*it).toObject()["code"].toString();
        newCourse->credit = (*it).toObject()["credit"].toDouble();
        newCourse->department = (*it).toObject()["department"].toString();
        newCourse->ratingAvg = (*it).toObject()["rating"].toObject()["avg"].toDouble();
        newCourse->ratingCount = (*it).toObject()["rating"].toObject()["count"].toInt();

        QString teacherName = (*it).toObject()["teacher"].toString();
        Teacher *newTeacher = mAppModel->coreData()->addTeacher(teacherName,&ok);
        if(ok){
            //只有教师添加成功才更新教师拼音
            newTeacher->pinyin = Pinyin::getFullChars(teacherName);
            newTeacher->abbrPinyin = Pinyin::getCamelChars(teacherName).join("");
        }

        mAppModel->coreData()->addMapping(newTeacher,newCourse);
    }
}

void Client::logout()
{
    getApiData(LOGOUT_URL);
    //清除数据
    mAppModel->clearData();
    switchLoginWindow();
}

QNetworkReply *Client::getApiReply(const QUrl &apiUrl)
{
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = nullptr;
    while(true){
        reply = mManager->get(request);
        mEventLoop->exec();
        if(reply->error() == QNetworkReply::NoError){
            //获取资源成功
            //reply需要caller手动销毁
            return reply;
        }
        //获取资源失败
        if(autoUpdateCookies()){
            //重新自动获取Cookies成功
            delete reply;
            continue;
        }
        qDebug() << "请求失败";
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << QString::fromUtf8(reply->readAll());
        delete reply;
        //TODO : 登出
        return nullptr;
    }
}

QByteArray Client::getApiData(const QUrl &apiUrl, bool *ok)
{
    CustomNetworkReply* historyReply = mAppModel->networkReplyHistory()->findNetworkReply(apiUrl.toString());
    if(historyReply) {
        //最近请求过相同的api
        qDebug() << "recent request...";
        return historyReply->replyData;
    }


    QNetworkReply* reply = getApiReply(apiUrl);
    if(ok) {
        *ok = reply;
    }
    if(reply) {
        //请求成功
        QByteArray replyData = reply->readAll();
        mAppModel->networkReplyHistory()->addNetworkReplyHistory(reply->request().url().toString() , replyData);
        delete reply;
        return replyData;
    }
    else {
        //请求失败
        return "";
    }
}


bool Client::autoUpdateCookies()
{
    //清除之前的Cookies
    mAppModel->cookieJarPtr()->clear();

    //更新Cookies
    QNetworkRequest loginRequest;
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;
    if(mAppModel->loginMode() == "emailPasswordLogin"){
        //使用邮箱密码登录
        loginRequest.setUrl(EMAIL_LOGIN_URL);
        postData.addQueryItem("account",mAppModel->account().account);
        postData.addQueryItem("password",mAppModel->account().password);
    }
    else if(mAppModel->loginMode() == "userPasswordLogin"){
        //使用账号密码登录
        loginRequest.setUrl(LOGIN_URL);
        postData.addQueryItem("username",mAppModel->account().account);
        postData.addQueryItem("password",mAppModel->account().password);
    }
    else{
        //使用其他方式登录，无法自动更新Cookies
        return false;
    }
    QNetworkReply* reply = mManager->post(loginRequest,postData.toString(QUrl::FullyEncoded).toUtf8());
    mEventLoop->exec();
    auto error = reply->error();
    delete reply;
    return error == QNetworkReply::NoError;
}

