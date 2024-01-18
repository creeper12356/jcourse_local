#ifndef USERHEAD_H
#define USERHEAD_H
#include "inc.h"

class UserHead : public QWidget
{
    //用户头像
    Q_OBJECT
public:
    explicit UserHead(QWidget *parent = nullptr);
public:
    void setUserName(const QString& userName);
protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);
private:
    QColor generateUserColor(const QString& name);

private:
    QString mUserName;
    QColor mBgColor;
    QPixmap mPicture;

signals:

};

#endif // USERHEAD_H
