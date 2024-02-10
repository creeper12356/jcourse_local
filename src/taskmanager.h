#ifndef TASKMANAGER_H
#define TASKMANAGER_H
#include "inc.h"
namespace taskManager {
enum type {search, checkReview, cacheReview, cacheCourseReview, cacheCourseCodeReview, cacheCourseCodeReviewPart};
}
class SearchTask;
class CheckReviewTask;
class CacheCourseReviewTask;
class CacheReviewTask;
class CacheCourseCodeReviewTask;
class CacheCourseCodeReviewPartTask;

class Task
{
public:
    taskManager::type type;
    bool isCompound;

    explicit Task(taskManager::type type, bool isCompound);
    virtual ~Task();


    //向下转化函数，须确保类型后安全转换
    SearchTask* toSearchTask();
    CheckReviewTask* toCheckReviewTask();
    CacheCourseReviewTask* toCacheCourseReviewTask();
    CacheReviewTask* toCacheReviewTask();
    CacheCourseCodeReviewTask* toCacheCourseCodeReviewTask();

    const SearchTask* toSearchTask() const;
    const CheckReviewTask* toCheckReviewTask() const;
    const CacheCourseReviewTask* toCacheCourseReviewTask() const;
    const CacheReviewTask* toCacheReviewTask() const;
    const CacheCourseCodeReviewTask* toCacheCourseCodeReviewTask() const;
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

class CacheCourseCodeReviewPartTask : public SingleTask
{
public:
    QString courseCode;
    int page;
    bool isLastPage;
    CacheCourseCodeReviewPartTask(const QString& courseCode, int page , bool isLastPage);
};

class CacheReviewTask : public SingleTask
{
public:
    int courseid;
    int page;
    bool isLastPage;
    CacheReviewTask(int courseid, int page, bool isLastPage);
};

class CacheCourseCodeReviewTask: public CompoundTask
{
public:
    QString courseCode;
    CacheCourseCodeReviewTask(const QString& courseCode);
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
    void CacheCourseCodeReviewPartFinished(QByteArray result, QString courseCode, int page, bool isLastPage);
private:
    QVector<Task*> mTasks;
    QNetworkAccessManager* mNetworkAccessManager;
};

#endif // TASKMANAGER_H
