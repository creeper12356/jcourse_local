#include "taskmanager.h"
#include "paginationwidget.h"

using namespace taskManager;

TaskManager::TaskManager(QObject *parent, QNetworkAccessManager *networkAccessManager)
    : QObject(parent)
    , mNetworkAccessManager(networkAccessManager)
{
    connect(mNetworkAccessManager,&QNetworkAccessManager::finished,this,&TaskManager::handleNetworkReply);
    connect(this,&TaskManager::taskQueueUpdated,this,&TaskManager::handleTaskQueueUpdated);
//    QTimer* timer = new QTimer(this);
//    connect(timer,&QTimer::timeout,this,[this]() {
//        qDebug() << mTasks.size();
//    });
//    timer->start(10);
}

TaskManager::~TaskManager()
{
    qDebug() << "delete";
    for(Task* task : mTasks) {
        delete task;
    }
}

void TaskManager::appendTask(Task *newTask)
{
    assert(newTask);
    mTasks.push_back(newTask);
    qDebug() << "append: " << mTasks.size();
    notifyTaskQueueUpdated();
}

void TaskManager::prependTask(Task *newTask)
{
    assert(newTask);
    //manually fix bug
    if(!mTasks.isEmpty() && !mTasks[0]->isCompound && dynamic_cast<SingleTask*>(mTasks[0])->isDoing) {
        qDebug() << "insert at second place";
        mTasks.insert(1,newTask);
    }
    else {
        qDebug() << "normal prepend";
        mTasks.prepend(newTask);
    }
    qDebug() << "prepend: " << mTasks.size();
    notifyTaskQueueUpdated();
}

void TaskManager::handleTaskQueueUpdated()
{
    qDebug() << "handleTaskQueueUpdated";
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
                delete mTasks[0];
                mTasks.removeFirst();
                notifyTaskQueueUpdated();
                break;
            }
            default:
                break;
            }
        }
        else {
            SingleTask* singleTask = dynamic_cast<SingleTask*>(mTasks[0]);
            if(singleTask->isDoing) {
                qDebug() << "first task doing . so skip.";
                return ;
            }
            singleTask->isDoing = true;
            QNetworkRequest request(singleTask->url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            mNetworkAccessManager->get(request);
        }
    }
}

void TaskManager::handleNetworkReply(QNetworkReply *reply)
{
    qDebug() << "handleNetworkReply";
    SingleTask* targetTask = nullptr;
    for(Task* task : mTasks) {
        if(task->isCompound) {
            //跳过复合请求
            continue;
        }
        if(dynamic_cast<SingleTask*>(task)->url == reply->request().url()) {
            //按时间顺序查找请求，按照url匹配
            targetTask = dynamic_cast<SingleTask*>(task);
            break;
        }
    }

//    assert(targetTask);
    if(!targetTask) {
        qDebug() << "other network reply";
        return ;
    }

    auto taskType = targetTask->type;

    QByteArray replyData = reply->readAll();
    //TODO : reply.error?
    if(reply->error() != QNetworkReply::NoError) {
        if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 429) {
            //使用正则表达式解析等待的时间
            QRegularExpression regex("\\d+");
            QRegularExpressionMatchIterator matchIterator = regex.globalMatch(QString::fromUtf8(replyData));
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                QString matchedText = match.captured();
                int waitTime = matchedText.toInt() * 1000;
                qDebug() << "wait for " << waitTime << " ms.";
                QTimer::singleShot(waitTime,this,&TaskManager::notifyTaskQueueUpdated);
                break;
            }
        }

        delete reply;
        return ;
    }
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
    notifyTaskQueueUpdated();
    delete targetTask;
    delete reply;
}

void TaskManager::notifyTaskQueueUpdated()
{
    emit taskQueueUpdated(&mTasks);
}

Task::Task(taskManager::type arg_type, bool arg_isCompound)
    : type(arg_type)
    , isCompound(arg_isCompound)
{

}

Task::~Task()
{

}

QJsonObject Task::toJsonObject() const
{
    QJsonObject taskJsonObject;
    taskJsonObject.insert("type",type);
    taskJsonObject.insert("isCompound",isCompound);
    return taskJsonObject;
}

SearchTask *Task::toSearchTask()
{
    return dynamic_cast<SearchTask*>(this);
}

CheckReviewTask *Task::toCheckReviewTask()
{
    return dynamic_cast<CheckReviewTask*>(this);
}

CacheCourseReviewTask *Task::toCacheCourseReviewTask()
{
    return dynamic_cast<CacheCourseReviewTask*>(this);
}

CacheReviewTask *Task::toCacheReviewTask()
{
    return dynamic_cast<CacheReviewTask*>(this);
}

const SearchTask *Task::toSearchTask() const
{
    return dynamic_cast<const SearchTask*>(this);
}

const CheckReviewTask *Task::toCheckReviewTask() const
{
    return dynamic_cast<const CheckReviewTask*>(this);
}

const CacheCourseReviewTask *Task::toCacheCourseReviewTask() const
{
    return dynamic_cast<const CacheCourseReviewTask*>(this);
}

const CacheReviewTask *Task::toCacheReviewTask() const
{
    return dynamic_cast<const CacheReviewTask*>(this);
}

SearchTask::SearchTask(const QString &arg_query, int arg_page)
    : SingleTask(taskManager::search, QUrl(SEARCH_URL(arg_query,arg_page)))
    , query(arg_query)
    , page(arg_page)
{

}

QJsonObject SearchTask::toJsonObject() const
{
    QJsonObject taskJsonObject = SingleTask::toJsonObject();
    taskJsonObject.insert("query",query);
    taskJsonObject.insert("page",page);

    return taskJsonObject;
}

CheckReviewTask::CheckReviewTask(int arg_courseid, int arg_page)
    : SingleTask(taskManager::checkReview,QUrl(REVIEW_URL(arg_courseid,arg_page)))
    , courseid(arg_courseid)
    , page(arg_page)
{

}

QJsonObject CheckReviewTask::toJsonObject() const
{
    QJsonObject taskJsonObject = SingleTask::toJsonObject();
    taskJsonObject.insert("courseid",courseid);
    taskJsonObject.insert("page",page);

    return taskJsonObject;
}

CacheReviewTask::CacheReviewTask(int arg_courseid, int arg_page)
    : SingleTask(taskManager::cacheReview, QUrl(REVIEW_URL(arg_courseid,arg_page)))
    , courseid(arg_courseid)
    , page(arg_page)
{

}

QJsonObject CacheReviewTask::toJsonObject() const
{
    QJsonObject taskJsonObject = SingleTask::toJsonObject();
    taskJsonObject.insert("courseid",courseid);
    taskJsonObject.insert("page",page);

    return taskJsonObject;
}

CacheCourseReviewTask::CacheCourseReviewTask(int arg_courseid)
    : CompoundTask(taskManager::cacheCourseReview)
    , courseid(arg_courseid)
{

}

QJsonObject CacheCourseReviewTask::toJsonObject() const
{
    QJsonObject taskJsonObject = CompoundTask::toJsonObject();
    taskJsonObject.insert("courseid",courseid);

    return taskJsonObject;
}

SingleTask::SingleTask(taskManager::type arg_type, const QUrl &arg_url)
    : Task(arg_type,false)
    , url(arg_url)
{

}

SingleTask::~SingleTask()
{

}

QJsonObject SingleTask::toJsonObject() const
{
    QJsonObject taskJsonObject = Task::toJsonObject();
    taskJsonObject.insert("url",url.toString());
    taskJsonObject.insert("isDoing",isDoing);
    return taskJsonObject;
}

CompoundTask::CompoundTask(taskManager::type arg_type)
    : Task(arg_type,true)
{

}

CompoundTask::~CompoundTask()
{

}

QJsonObject CompoundTask::toJsonObject() const
{
    return Task::toJsonObject();
}
