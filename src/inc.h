#ifndef INC_H
#define INC_H

#include <QMainWindow>
#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>

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

#include <QFile>
#include <QDir>

#include <QDebug>


#define LOGIN_URL QUrl("https://course.sjtu.plus/oauth/email/login/")
#define SEARCH_URL(query) QUrl(QString("https://course.sjtu.plus/api/search/?q=%1&page=1&size=20").arg(query))

class MainWindow;
class LoginWindow;
class Account;

#endif // INC_H
