#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "inc.h"

namespace Ui {
class MainWindow;
}

/*!
 * \brief 主窗口
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    /*!
     * \brief 显示搜索课程的结果
     *
     * Client::searchFinished的槽函数，将客户端发出的搜索结果解析显示到界面。
     * \param 客户端发出信号的参数，搜索结果字节流，使用JSON格式
     * \sa Client::searchFinished
     */
    void displaySearchResult(QByteArray result);

    /*!
     * \brief 显示搜索课程评价的结果
     *
     * Client::checkReviewFinished的槽函数，将客户端发出的搜索结果解析显示到界面。
     * \param 客户端发出信号的参数，搜索结果字节流，使用JSON格式
     * \sa Client::checkReviewFinished
     */
    void displayCheckReviewResult(QByteArray result);


    void displayParseCourseStatusResult(QJsonObject resultJsonObject);

    /*!
     * \brief 用户名改变的槽函数
     *
     * AppModel::userNameChanged的槽函数，将新更改的用户名更新到界面上。
     * \param userName 新的用户名
     * \sa AppModel::userNameChanged
     */
    void userNameChangedSlot(QString userName);

    /*!
     * \brief 在线模式改变的槽函数
     *
     * AppModel::onlineChanged的槽函数，将更新后的在线/离线模式更新到界面上
     * \param isOnline 是否为在线模式
     * \sa AppModel::onlineChanged
     */
    void onlineChangedSlot(bool isOnline);

    /*!
     * \brief 登录模式改变的槽函数
     * \sa AppModel::loginModeChanged
     */
    void loginModeChangedSlot(QString loginMode);

private slots:
    /*!
     * \brief 用户使用搜索框搜索课程
     *
     * 当用户使用搜索框搜索课程时，该槽函数被调用，并发送search信号。
     * \sa search
     */
    void searchBarTriggered();

    /*!
     * \brief 用户请求换页查看课程
     *
     * 当用户请求换页查看搜索的课程时，该槽函数被调用，并发送search信号。
     * \param page 用户请求的页数
     * \sa search
     */
    void searchPageTriggered(int page);

    /*!
     * \brief 用户请求查看课程评价
     *
     * 当用户点击课程条目时，该槽函数被调用，并发送checkReview信号。
     * \param courseid 请求的课程id
     * \sa checkReview
     */
    void checkReviewTriggered(int courseid);

    /*!
     * \brief 用户请求换页查看课程评价
     *
     * 当用户请求换页查看课程评价时，该槽函数被调用，并发送checkReview信号。
     * \param page 用户请求的页数
     * \sa checkReview
     */
    void checkReviewPageTriggered(int page);

    void parseCourseStatusTriggered();

protected:
    /*!
     * \brief 主窗口每次出现初始化
     */
    void showEvent(QShowEvent *event);
signals:
    /*!
     * \brief 搜索课程信号
     *
     * 向Client发送的搜索课程信号，请求搜索{query}对应的所有课程信息。
     * \param query 搜索内容
     * \param page 搜索页数（从1开始）
     */
    void search(QString query,int page);

    /*!
     * \brief 查看课程评价信号
     *
     * 向Client发送的查看课程评价的信号，请求查看指定课程id，指定页数的课程评价。
     * \param courseid 课程的id
     * \param page 搜索页数（从1开始）
     */
    void checkReview(int courseid,int page);

    /*!
     * \brief 在线模式改变信号
     *
     * 向AppModel发送的在线模式改变信号，AppModel收到该信号后将修改后台数据。
     * \param checked 是否为在线模式
     */
    void changeOnline(bool checked);


    void parseCourseStatus(QString src);

    //发送给Client的缓存课程评价信号
    void cacheCourseReview(int courseid);

    //发送给Client的缓存所有课号为courseCode的课程评价信号
    void cacheCourseCodeReview(QString courseCode);

    /*!
     * \brief 登出信号
     *
     * 向Client发送的登出信号，请求客户端将当前账号登出。
     */
    void logout();


private:
    QString mLastQuery;//!<上一次的搜索内容
    int mLastCourseid;//!<上一次查看课程评价的课程id
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
