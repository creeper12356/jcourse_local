#include "taskmanager.h"
#include "paginationwidget.h"

using namespace taskManager;

TaskManager::TaskManager(QObject *parent, QNetworkAccessManager *networkAccessManager)
    : QObject(parent)
    , mNetworkAccessManager(networkAccessManager)
    , mMaxCacheReviewId(0)
{
    connect(mNetworkAccessManager,&QNetworkAccessManager::finished,this,&TaskManager::handleNetworkReply);
    connect(this,&TaskManager::taskQueueUpdated,this,&TaskManager::handleTaskQueueUpdated);
//    QTimer* timer = new QTimer(this);
//    connect(timer,&QTimer::timeout,this,[this]() {
//        qDebug() << mTasks;
//    });
//    timer->start(10);
}

TaskManager::~TaskManager()
{
    for(Task* task : mTasks) {
        delete task;
    }
}

void TaskManager::addTask(Task *newTask)
{
    assert(newTask);
    mTasks.push_back(newTask);
    emit taskQueueUpdated();
}

void TaskManager::handleTaskQueueUpdated()
{
    qDebug() << "handle task queue updated";
    if(mTasks.empty()){
        qDebug() << "task queue now empty";
        return ;
    }
    else {
        if(mTasks[0]->isCompound) {
            switch(mTasks[0]->type) {
            case cacheCourseReview:
            {
                mTasks.push_back(new CacheReviewTask(dynamic_cast<CacheCourseReviewTask*>(mTasks[0])->courseid,1));
                mTasks.removeFirst();
                emit taskQueueUpdated();
                break;
            }
            default:
                break;
            }
        }
        else {
            QNetworkRequest request(dynamic_cast<SingleTask*>(mTasks[0])->url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            mNetworkAccessManager->get(request);
        }

    }
}

void TaskManager::handleNetworkReply(QNetworkReply *reply)
{
    SingleTask* targetTask = nullptr;
    for(Task* task : mTasks) {
        if(task->isCompound) {
            //跳过复合请求
            continue;
        }
        assert(!task->isCompound);
        if(dynamic_cast<SingleTask*>(task)->url == reply->request().url()) {
            //按时间顺序查找请求，按照url匹配
            targetTask = dynamic_cast<SingleTask*>(task);
            break;
        }
    }

    assert(targetTask);

    auto taskType = targetTask->type;
    //TODO : reply.error?
    if(reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->error();
        qDebug() << reply->readAll();
        assert(false);
    }
    QByteArray replyData = reply->readAll();

    switch(taskType) {
    case search:
    {
        auto searchTask = dynamic_cast<SearchTask*>(targetTask);
        emit searchFinished(replyData,searchTask->query,searchTask->page);
        break;
    }
    case checkReview:
    {
        auto checkReviewTask = dynamic_cast<CheckReviewTask*>(targetTask);
        emit checkReviewFinished(replyData,checkReviewTask->courseid,checkReviewTask->page);
        break;
    }
    case cacheReview:
    {
        auto cacheReviewTask = dynamic_cast<CacheReviewTask*>(targetTask);
        if(cacheReviewTask->page == 1) {
            int pageCount = PaginationWidget::divideTotal(QJsonDocument::fromJson(replyData).object()["count"].toInt(),PAGE_SIZE);
            for(int i = 2;i <= pageCount;++i) {
                //添加剩余下载请求
                mTasks.push_back(new CacheReviewTask(cacheReviewTask->courseid,i));
            }
        }
        emit cacheReviewTaskFinished(replyData,cacheReviewTask->courseid,cacheReviewTask->page);
        break;
    }
    default:
        break;
    }

    mTasks.removeOne(targetTask);
    emit taskQueueUpdated();
    delete targetTask;
    delete reply;
}

Task::Task(taskManager::type arg_type, bool arg_isCompound)
    : type(arg_type)
    , isCompound(arg_isCompound)
{

}

Task::~Task()
{

}

SearchTask::SearchTask(const QString &arg_query, int arg_page)
    : SingleTask(taskManager::search,QUrl(SEARCH_URL(arg_query,arg_page)))
    , query(arg_query)
    , page(arg_page)
{

}

CheckReviewTask::CheckReviewTask(int arg_courseid, int arg_page)
    : SingleTask(taskManager::checkReview,QUrl(REVIEW_URL(arg_courseid,arg_page)))
    , courseid(arg_courseid)
    , page(arg_page)
{

}

CacheReviewTask::CacheReviewTask(int arg_courseid, int arg_page)
    : SingleTask(taskManager::cacheReview, QUrl(REVIEW_URL(arg_courseid,arg_page)))
    , courseid(arg_courseid)
    , page(arg_page)
{

}

CacheCourseReviewTask::CacheCourseReviewTask(int arg_courseid)
    : CompoundTask(taskManager::cacheCourseReview)
    , courseid(arg_courseid)
{

}

SingleTask::SingleTask(taskManager::type arg_type, const QUrl &arg_url)
    : Task(arg_type,false)
    , url(arg_url)
{

}

SingleTask::~SingleTask()
{

}

CompoundTask::CompoundTask(taskManager::type arg_type)
    : Task(arg_type,true)
{

}

CompoundTask::~CompoundTask()
{

}
