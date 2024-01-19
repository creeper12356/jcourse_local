#include "client.h"
#include "mainwindow.h"
#include "loginwindow.h"
#include "pinyin.h"

#include "appmodel.h"

Client::Client(QApplication *app)
    : QObject(nullptr)
    , mApp(app)
{
    mEventLoop = new QEventLoop(this);
    mManager = new QNetworkAccessManager(this);

    mLoginWindow = new LoginWindow(mManager , nullptr);
    mMainWindow = new MainWindow(nullptr);

    mAppModel = new AppModel(mMainWindow);

    connect(mLoginWindow,&LoginWindow::emailPasswordLoginSuccess,this,[this](QString account , QString password){
        mAppModel->setAccountAndNotify(account,password);
        switchMainWindow();
    });
    connect(mLoginWindow,&LoginWindow::emailCodeLoginSuccess,this,[this](QString account){
        //TODO : 使用验证码登录，密码为空
        mAppModel->setAccountAndNotify(account,"");
        switchMainWindow();
    });

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

bool Client::search(const QString &query,int page)
{
    QByteArray replyData;
    if(mAppModel->isOnline()){
        //在线模式
        //请求api,缓存资源到本地
        auto reply = getWithCookies(SEARCH_URL(query,page));
        if(!reply){
            //在线获取资源失败
            return false;
        }
        replyData = reply->readAll();
        delete reply;
        QJsonObject resultJsonObject = QJsonDocument::fromJson(replyData).object();
        QJsonArray resultsJsonArray = resultJsonObject["results"].toArray();

        bool ok;

        for(auto it = resultsJsonArray.begin();it != resultsJsonArray.end(); ++it){

            Course *newCourse = mAppModel->coreData()->addCourse((*it).toObject()["id"].toInt(),(*it).toObject()["name"].toString());
            //无论课程之前是否存在，更新课程数据
            newCourse->code = (*it).toObject()["code"].toString();
            newCourse->credit = (*it).toObject()["credit"].toInt();
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
        qDebug() << REVIEW_URL(courseid,page);
        auto reply = getWithCookies(REVIEW_URL(courseid,page));
        if(!reply){
            return false;
        }
        //TODO : check reply error?
        replyData = reply->readAll();

        //缓存资源
        QFile downloader;
        downloader.setFileName(cacheReviewFileName);
        qDebug() << "cache to file: " << downloader.fileName();
        downloader.open(QIODevice::WriteOnly);
        downloader.write(replyData);
        downloader.close();

        delete reply;
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

void Client::logout()
{
    auto reply = getWithCookies(LOGOUT_URL);
    if(reply){
        delete reply;
    }
    //清除数据
    mAppModel->clearData();
    switchLoginWindow();
}

QNetworkReply *Client::getWithCookies(const QUrl &apiUrl)
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
        //只有账号密码登录条件下（密码不为空），才自动更新Cookies
        if(!mAppModel->account().password.isEmpty() &&  autoUpdateCookies()){
            //重新自动获取Cookies成功
            delete reply;
            continue;
        }
        qDebug() << "该cookies存在问题";
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << QString::fromUtf8(reply->readAll());
        delete reply;
        //TODO : 登出
        return nullptr;
    }
}

bool Client::autoUpdateCookies()
{
    //清除之前的Cookies
    mAppModel->cookieJarPtr()->clear();

    //更新Cookies
    QNetworkRequest loginRequest(EMAIL_LOGIN_URL);
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery postData;
    //TODO : assert not empty
    postData.addQueryItem("account",mAppModel->account().account);
    postData.addQueryItem("password",mAppModel->account().password);
    QNetworkReply* reply = mManager->post(loginRequest,postData.toString(QUrl::FullyEncoded).toUtf8());
    mEventLoop->exec();
    auto error = reply->error();
    delete reply;
    return error == QNetworkReply::NoError;
}

