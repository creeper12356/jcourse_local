#ifndef INC_H
#define INC_H

#include <QMainWindow>
#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QToolBar>
#include <QLabel>
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


#define LOGIN_URL QUrl("https://course.sjtu.plus/oauth/email/login/")
#define SEARCH_URL(query) QUrl(QString("https://course.sjtu.plus/api/search/?q=%1&page=1&size=20").arg(query))

class MainWindow;
class LoginWindow;
class Account;
class MyNetworkCookieJar;
class AppModel;

#endif // INC_H
