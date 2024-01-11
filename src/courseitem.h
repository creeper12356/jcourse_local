#ifndef COURSEITEM_H
#define COURSEITEM_H

#include "inc.h"

class CourseItem : public QListWidgetItem
{
    //课程条目
public:
    CourseItem();
    virtual ~CourseItem();
public:
    void updateCourseInfo(const QJsonObject &courseJsonObject);
    //将条目添加到列表view中
    void addToList(QListWidget* list);
private:
    QLabel *mTitleLabel = nullptr;
    QLabel *mDetailLabel = nullptr;
    QLabel *mRatingAvgLabel = nullptr;
    QLabel *mRatingCountLabel = nullptr;
    QWidget* mWidget = nullptr;

signals:


};

#endif // COURSEITEM_H
