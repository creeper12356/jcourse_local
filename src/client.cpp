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
    auto reply = getWithCookies(SEARCH_URL(query,page));
    //TODO : no error
    emit searchFinished(reply->readAll());
    delete reply;
    return true;
}

bool Client::checkReview(int courseid, int page)
{
    qDebug() << REVIEW_URL(courseid,page);
    auto reply = getWithCookies(REVIEW_URL(courseid,page));
    //TODO : no error
    emit checkReviewFinished(reply->readAll());
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
                mAppModel->setAccount(mLoginWindow->getAccount(),mLoginWindow->getPassword());
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
