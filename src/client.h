#ifndef CLIENT_H
#define CLIENT_H
#include "inc.h"
/*!
 * \brief 客户端
 *
 * 应用的控制器，包含所有核心逻辑。
*/

class Client : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief 构造函数
     * \param app 事件循环对应的QApplication指针
     */
    explicit Client(QApplication* app);

    ~Client();
public:

    /*!
     * \brief 切换到主窗口
     */
    void switchMainWindow();

    /*! * \brief 切换到登录窗口
     */
    void switchLoginWindow();

public slots:
    void emailPasswordLoginSuccess(QString account , QString password);
    void emailCodeLoginSuccess(QString account);
    void userPasswordLoginSuccess(QString user , QString password);
    void offlineLoginSuccess();

public:
    void warning(const QString& msg);
public slots:

    /*!
     * \brief 登出
     */
    void logout();

    /*!
     * \brief 查询课程
     *
     * 如果为在线模式，请求url : https://course.sjtu.plus/api/search/?q={query}&page={page}&size=20 ;
     * 如果为离线模式，从本地缓存文件coredata.json中读取资源。
     *
     * \param query 查询内容
     * \param page 查询页面（从1开始）
     * \return 是否查询成功
     */
    bool search(const QString& query, int page = 1);


    /*!
     * \brief 查看课程评价
     *
     * 如果为在线模式，请求url : https://course.sjtu.plus/api/course/{courseid}/review/?&page={page}&size=20;
     * 如果为离线模式，从本地缓存文件cache/review_{courseid}_{page}.json中读取资源。
     *
     * \param courseid 选课社区使用的课程id
     * \param page 查询页面（从1开始）
     * \return 是否查询成功
     */
    bool checkReview(int courseid, int page = 1);

    /*!
     * \brief 解析修业情况查询结果
     *
     * 对修业情况查询结果进行解析，解析完成后发送parseCourseStatusFinished信号给界面，将解析结果显示在列表中。
     * \param src 待解析的字符串
     * \sa parseCourseStatusFinished
     */
    void parseCourseStatus(QString src);

    /*!
     * \brief 缓存课程评价
     *
     * 将指定课程的所有评价都缓存入文件，调用此函数要求客户端为在线模式。
     * \param courseid 缓存的课程id
     * \sa cacheCourseCodeReview
     */
    void cacheCourseReview(int courseid);

    /*!
     * \brief 缓存指定课号的所有课程评价
     *
     * 将指定课号的所有课程的所有评价都缓存入文件，调用此函数要求客户端为在线模式
     * \param courseCode 缓存的课号
     * \sa cacheCourseReview
     */
    void cacheCourseCodeReview(QString courseCode);
public slots:
    //TODO : rename ...
    //与TaskManager信号连接的槽
    void searchFinishedHandler(QByteArray result);
    void checkReviewFinishedHandler(QByteArray result,int courseid,int page);
    void cacheReviewFinishedHandler(QByteArray result,int courseid,int page,bool isLastPage);
public:

    /*!
     * \brief 发送带Cookies的GET请求，获得服务器回应对象
     * \param apiUrl 请求的url
     * \return 如果请求成功，返回服务器的回应(QNetworkReply*)，指针指向的内存需要调用者手动释放；
     *         如果请求失败，返回nullptr。
     * \sa getApiData
     */
    QNetworkReply *getApiReply(const QUrl &apiUrl);

    /*!
     * \brief 发送带Cookies的GET请求，获得服务器回应字节流
     * \param apiUrl 请求的url
     * \param ok 请求结果
     * \return 当ok != 0时，通过*ok返回请求是否成功。
     * 	       如果请求成功，返回服务器回应的字节流；
     * 	       如果请求失败，返回""；
     * \sa getApiReply
     */
    QByteArray getApiData(const QUrl &apiUrl, bool* ok = nullptr);

    /*!
     * \brief 在线搜索课程
     * \param query 搜索内容
     * \param page 搜索页面
     * \return 如果请求成功，返回服务器回应的JSON格式字节流；
     * 如果请求失败，返回空。
     */
    QByteArray getSearchResult(const QString& query , int page);

    /*!
     * \brief 在线请求课程评价
     * \param courseid 课程id
     * \param page 请求页面
     * \return 如果请求成功，返回服务器回应的JSON格式字节流；
     * 如果请求失败，返回空。
     */
    QByteArray getCourseReview(int courseid, int page);

    /*!
     * \brief 将搜索结果缓存
     * \param replyData 搜索结果字节流
     */
    void cacheSearchResult(const QByteArray& replyData);
    /*!
     * \brief 将搜索结果缓存
     * \param replyJsonObject 解析为QJsonObject的搜索结果
     */
    void cacheSearchResult(const QJsonObject& replyJsonObject);



    /*!
     * \brief 将服务器回应数据写入文件
     * \param replyData 服务器回应的字节流
     * \param fileName 写入文件路径
     */
    void cacheReplyData(const QByteArray& replyData , const QString& fileName);

signals:

    /*!
     * \brief 查询课程成功后，发送给MainWindow的信号
     * \param result 查询结果字节流（JSON格式）
     */
    void searchFinished(QByteArray result);

    /*!
     * \brief 查询课程评价成功后，发送给MainWindow的信号
     * \param result 查询结果字节流（JSON格式）
     */
    void checkReviewFinished(QByteArray result);

    void parseCourseStatusFinished(QJsonObject resultJsonObject);

    /*!
     * \brief 缓存评价完成时，发送给MainWindow的信号
     * \param courseid 课程id
     * \param page 缓存页面
     */
    void cacheReviewFinished(int courseid,int page);

    /*!
     * \brief 缓存给定课程id的课程评价完成时，发送给MainWindow的信号
     * \param courseid 课程id
     */
    void cacheCourseReviewFinished(int courseid);

private:
    /*!
     * \brief 自动更新过期的Cookies
     * \return 是否更新成功
     */
    bool autoUpdateCookies();

private:
    LoginWindow* mLoginWindow = nullptr;
    MainWindow* mMainWindow = nullptr;
private:
    QApplication *mApp = nullptr;
    QEventLoop *mEventLoop = nullptr;
    QNetworkAccessManager* mNetworkAccessManager = nullptr;
    TaskManager* mTaskManager = nullptr;
    AppModel* mAppModel = nullptr;

};

#endif // CLIENT_H
