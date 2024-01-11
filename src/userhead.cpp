#include "userhead.h"

UserHead::UserHead(QWidget *parent) : QWidget(parent)
{
    setUserName("");
}

void UserHead::setUserName(const QString &userName)
{
    mUserName = userName;
    setToolTip(mUserName);
    mBgColor = generateUserColor(mUserName);
    update();
}

void UserHead::paintEvent(QPaintEvent *event)
{
    int size = qMax(width() , height());
    QPainter painter(this);
    painter.setBrush(mBgColor);
    painter.setPen(Qt::white);
    painter.drawEllipse(0,0,size,size);
    if(!mUserName.isEmpty()){
        painter.drawText(QRect(0,0,size,size),Qt::AlignCenter,QString(mUserName[0].toUpper()));
    }

}

bool UserHead::event(QEvent *event)
{
    if(event->type() == QEvent::Enter){
        qDebug() << mUserName;
        return true;
    }
    return QWidget::event(event);
}

QColor UserHead::generateUserColor(const QString& name) {
    // 计算用户名的哈希值，这里简单地使用qHash函数
    uint hashValue = qHash(name);

    // 提取哈希值的红、绿、蓝分量
    int red = (hashValue & 0xFF0000) >> 16;
    int green = (hashValue & 0x00FF00) >> 8;
    int blue = hashValue & 0x0000FF;

    // 创建并返回QColor对象
    return QColor(red, green, blue);
}
