#ifndef TASKMANAGER_H
#define TASKMANAGER_H
#include "inc.h"
namespace taskManager {
enum type {search, checkReview, cacheCourseReview, cacheReview};
}
class Task
{
public:
    taskManager::type type;
    bool isCompound;

    explicit Task(taskManager::type type, bool isCompound);
    virtual ~Task();
};
//!单一任务，直接发送网络请求
class SingleTask : public Task
{
public:
    QUrl url;
    bool isDoing = false;

    SingleTask(taskManager::type type,const QUrl& url);
    virtual ~SingleTask();
};

//!复合任务，被分解为多个单一任务，不直接发送网络请求
class CompoundTask : public Task
{
public:
    explicit CompoundTask(taskManager::type type);
    virtual ~CompoundTask();
};

class SearchTask : public SingleTask
{
public:
    QString query;
    int page;
    SearchTask(const QString& query,int page);
};

class CheckReviewTask : public SingleTask
{
public:
    int courseid;
    int page;
    CheckReviewTask(int courseid, int page);
};

class CacheCourseReviewTask: public CompoundTask
{
public:
    int courseid;
    CacheCourseReviewTask(int courseid);
};

class CacheReviewTask : public SingleTask
{
public:
    int courseid;
    int page;
    CacheReviewTask(int courseid, int page);
};

class TaskManager : public QObject
{
    Q_OBJECT
public:
    TaskManager(QObject* parent , QNetworkAccessManager* networkAccessManager);
    ~TaskManager();

    void appendTask(Task* newTask);
    void prependTask(Task* newTask);
public slots:
    void handleTaskQueueUpdated();
    void handleNetworkReply(QNetworkReply* reply);
signals:
    void taskQueueUpdated();
    void searchFinished(QByteArray result,QString query,int page);
    void checkReviewFinished(QByteArray result,int courseid,int page);
    void cacheReviewTaskFinished(QByteArray result, int courseid,int page);
private:
    QVector<Task*> mTasks;
    QNetworkAccessManager* mNetworkAccessManager;
};

#endif // TASKMANAGER_H
