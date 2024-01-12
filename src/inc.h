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
#define SEARCH_URL(query,page) \
    QUrl(QString("https://course.sjtu.plus/api/search/?q=%1&page=%2&size=%3")\
    .arg(query,QString::number(page),QString::number(PAGE_SIZE)))
#define REVIEW_URL(courseid,page) \
    QUrl(QString("https://course.sjtu.plus/api/course/%1/review/?&page=%2&size=%3")\
    .arg(QString::number(courseid),QString::number(page),QString::number(PAGE_SIZE)))

class MainWindow;
class LoginWindow;
class Account;
class MyNetworkCookieJar;
class AppModel;

#endif // INC_H
