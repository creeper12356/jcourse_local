#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "inc.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //搜索框被触发
    void searchBarTriggered();
    //换页按钮被触发
    void searchPageTriggered(int page);
    //用户触发查看课程评价
    void checkReviewTriggered(int courseid);
    //用户查看课程评价换页按钮被触发
    void checkReviewPageTriggered(int page);

public slots:
    //Client::searchFinished的槽函数
    void displaySearchResult(QByteArray result);
    //Client::checkReviewFinished的槽函数
    void displayCheckReviewResult(QByteArray result);

    //AppModel::userNameChanged的槽函数
    void userNameChangedSlot(QString userName);
    void onlineChangedSlot(bool isOnline);
signals:
    //检索信号，发送给Client
    void search(QString query,int page);
    //查看课程评价信号，发送给Client
    void checkReview(int courseid,int page);
    //改变客户端状态信号，发送给Client
    void changeOnline(bool checked);
private:
    //上一次的检索信息
    QString mLastQuery;
    //上一次查看评价的课程
    int mLastCourseid;
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
