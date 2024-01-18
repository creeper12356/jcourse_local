#ifndef INC_H
#define INC_H

#include <QMainWindow>
#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QToolBar>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QListWidget>
#include <QComboBox>

#include <QEventLoop>
#include <QApplication>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QUrl>
#include <QUrlQuery>

#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QFile>
#include <QDir>

#include <QDateTime>
#include <QTimer>

#include <QPainter>

#include <QDebug>

#define PAGE_SIZE 20

#define LOGIN_URL QUrl("https://course.sjtu.plus/oauth/email/login/")
#define LOGOUT_URL QUrl("https://course.sjtu.plus/oauth/logout/")

#define SEARCH_URL(query,page) \
    QUrl(QString("https://course.sjtu.plus/api/search/?q=%1&page=%2&size=%3")\
    .arg(query,QString::number(page),QString::number(PAGE_SIZE)))
#define REVIEW_URL(courseid,page) \
    QUrl(QString("https://course.sjtu.plus/api/course/%1/review/?&page=%2&size=%3")\
    .arg(QString::number(courseid),QString::number(page),QString::number(PAGE_SIZE)))

#define SEARCH_MESSAGE(query, page) \
    QString("搜索课程：q=%1,page=%2...").arg(query,QString::number(page))
#define REVIEW_MESSAGE(courseid,page) \
    QString("查看评价：id=%1,page=%2...").arg(QString::number(courseid),QString::number(page))
#define COURSE_INFO(courseCount) \
    (courseCount ? QString("共有%1门课").arg(courseCount) : "暂无数据")
#define REVIEW_INFO(reviewCount) \
    (reviewCount ? QString("点评（%1条）").arg(reviewCount) : "暂无点评")


//缓存评价文件名
//int courseid , int page
#define CACHE_REVIEW_BASENAME(courseid,page) \
        QString("review_%1_%2.json").arg(QString::number(courseid),QString::number(page))

class MainWindow;
class LoginWindow;
class Account;
class MyNetworkCookieJar;
class AppModel;
class PaginationWidget;
class AbstractItem;

#endif // INC_H
