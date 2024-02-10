#ifndef TASKMANAGER_H
#define TASKMANAGER_H
#include "inc.h"
namespace taskManager {
enum type {search, checkReview, cacheCourseReview, cacheReview};
}
class SearchTask;
class CheckReviewTask;
class CacheCourseReviewTask;
class CacheReviewTask;

class Task
{
public:
    taskManager::type type;
    bool isCompound;

    explicit Task(taskManager::type type, bool isCompound);
    virtual ~Task();

    virtual QJsonObject toJsonObject() const;

    //向下转化函数，须确保类型后安全转换
    SearchTask* toSearchTask();
    CheckReviewTask* toCheckReviewTask();
    CacheCourseReviewTask* toCacheCourseReviewTask();
    CacheReviewTask* toCacheReviewTask();

    const SearchTask* toSearchTask() const;
    const CheckReviewTask* toCheckReviewTask() const;
    const CacheCourseReviewTask* toCacheCourseReviewTask() const;
    const CacheReviewTask* toCacheReviewTask() const;
};
//!单一任务，直接发送网络请求
class SingleTask : public Task
{
public:
    QUrl url;
    bool isDoing = false;

    SingleTask(taskManager::type type,const QUrl& url);
    virtual ~SingleTask();

    virtual QJsonObject toJsonObject() const override;
};

//!复合任务，被分解为多个单一任务，不直接发送网络请求
class CompoundTask : public Task
{
public:
    explicit CompoundTask(taskManager::type type);
    virtual ~CompoundTask();

    virtual QJsonObject toJsonObject() const override;
};

class SearchTask : public SingleTask
{
public:
    QString query;
    int page;
    SearchTask(const QString& query,int page);

    QJsonObject toJsonObject() const override;
};

class CheckReviewTask : public SingleTask
{
public:
    int courseid;
    int page;
    CheckReviewTask(int courseid, int page);

    QJsonObject toJsonObject() const override;
};

class CacheCourseReviewTask: public CompoundTask
{
public:
    int courseid;
    CacheCourseReviewTask(int courseid);

    QJsonObject toJsonObject() const override;
};

class CacheReviewTask : public SingleTask
{
public:
    int courseid;
    int page;
    bool isLastPage;
    CacheReviewTask(int courseid, int page, bool isLastPage);

    QJsonObject toJsonObject() const override;
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

protected slots:
    //TODO : change name
    void notifyTaskQueueUpdated();
signals:
    void taskQueueUpdated();
    void searchFinished(QByteArray result,QString query,int page);
    void checkReviewFinished(QByteArray result,int courseid,int page);
    void cacheReviewFinished(QByteArray result, int courseid, int page, bool isLastPage);
private:
    QVector<Task*> mTasks;
    QNetworkAccessManager* mNetworkAccessManager;
};

#endif // TASKMANAGER_H
