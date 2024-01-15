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

    mLoginWindow = new LoginWindow(nullptr);
    mMainWindow = new MainWindow(nullptr);

    mAppModel = new AppModel(mMainWindow);

    connect(mMainWindow,&MainWindow::search,this,&Client::search);
    connect(this,&Client::searchFinished,mMainWindow,&MainWindow::displaySearchResult);

    connect(mMainWindow,&MainWindow::checkReview,this,&Client::checkReview);
    connect(this,&Client::checkReviewFinished,mMainWindow,&MainWindow::displayCheckReviewResult);

    connect(mManager,&QNetworkAccessManager::finished,mEventLoop,&QEventLoop::quit);
    connect(mLoginWindow,&LoginWindow::rejected,mApp,&QApplication::quit);
}

bool Client::initialize()
{
    mManager->setCookieJar(mAppModel->cookieJarPtr());
    mAppModel->readFromFile("./client.json");
    mAppModel->setCacheDirectory("cache");
    mMainWindow->show();
    return true;
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

bool Client::search(const QString &query,int page)
{
    QByteArray replyData;
    if(mAppModel->isOnline()){
        //在线模式
        //请求api,缓存资源到本地
        auto reply = getWithCookies(SEARCH_URL(query,page));
        //TODO : no error
        replyData = reply->readAll();
        delete reply;
        QJsonObject resultJsonObject = QJsonDocument::fromJson(replyData).object();
        QJsonArray resultsJsonArray = resultJsonObject["results"].toArray();

        for(auto it = resultsJsonArray.begin();it != resultsJsonArray.end(); ++it){

            Course *newCourse = mAppModel->coreData()->addCourse((*it).toObject()["id"].toInt(),(*it).toObject()["name"].toString());
            newCourse->code = (*it).toObject()["code"].toString();
            newCourse->credit = (*it).toObject()["credit"].toInt();
            newCourse->department = (*it).toObject()["department"].toString();
            newCourse->ratingAvg = (*it).toObject()["rating"].toObject()["avg"].toDouble();
            newCourse->ratingCount = (*it).toObject()["rating"].toObject()["count"].toInt();

            //TODO : get pinyin of Chinese
            Teacher *newTeacher = mAppModel->coreData()->addTeacher((*it).toObject()["teacher"].toString(),"");
            mAppModel->coreData()->addMapping(newTeacher,newCourse);
        }
    }
    else{
        //离线模式
        //本地资源搜索结果
        QVector<const Mapping*> courseMappings = mAppModel->coreData()->searchCourseMappings(query,query,query);
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

QNetworkReply *Client::getWithCookies(const QUrl &apiUrl)
{
    QNetworkRequest request(apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = nullptr;
    while(true){
        reply = mManager->get(request);
//        qDebug() << "before exec:";
        //设置超时时间
//        QTimer::singleShot(5000,mEventLoop,&QEventLoop::quit);
        mEventLoop->exec();
//        qDebug() << "after exec:";
        if(reply->error() == QNetworkReply::NoError){
            //获取资源成功
            break;
        }
        //获取资源失败
        qDebug() << "该cookies存在问题";
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << QString::fromUtf8(reply->readAll());

        if(!updateCookies()){
            mLoginWindow->show();
            mMainWindow->close();
            do{
                if(!mLoginWindow->login()){
                    //用户取消登录
                    //关闭程序
                    mLoginWindow->reject();
                    qDebug() << "canceled";
                }
                //获取账号密码
                mAppModel->setAccountAndNotify(mLoginWindow->getAccount(),mLoginWindow->getPassword());
                qDebug() << "登录账号： " << mAppModel->account().account;
            } while(!updateCookies());
            mMainWindow->show();
            mLoginWindow->accept();
        }

        delete reply;
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
    mLoginWindow->displayStatusCode(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    delete reply;
    return error == QNetworkReply::NoError;
}
