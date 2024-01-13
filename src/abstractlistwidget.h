#ifndef ABSTRACTLISTWIDGET_H
#define ABSTRACTLISTWIDGET_H

#include "inc.h"

//统一CourseListWidget和ReviewListWidget
class AbstractListWidget : public QListWidget
{
    Q_OBJECT
    //抽象列表控件类，支持连接翻页控件使用
public:
    explicit AbstractListWidget(QWidget* parent = 0);
private slots:
    virtual void itemClickedSlot(QListWidgetItem *item);
signals:
};

#endif // ABSTRACTLISTWIDGET_H
