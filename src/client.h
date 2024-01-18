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

    /*!
     * \brief 切换到登录窗口
     */
    void switchLoginWindow();

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

public:

    /*!
     * \brief 发送带Cookies的GET请求
     * \param apiUrl 请求的url
     * \return 如果请求成功，返回服务器的回应(QNetworkReply*)，指针指向的内存需要调用者手动释放；
     *         如果请求失败，返回nullptr，并发出login信号。
     */
    QNetworkReply *getWithCookies(const QUrl &apiUrl);

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
    QNetworkAccessManager* mManager = nullptr;
    AppModel* mAppModel = nullptr;

};

#endif // CLIENT_H
