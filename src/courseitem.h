#ifndef COURSEITEM_H
#define COURSEITEM_H

#include "inc.h"
#include "abstractitem.h"

class CourseItem : public AbstractItem
{
    //课程条目
public:
    CourseItem();
    virtual ~CourseItem();
public:
    virtual void updateItemInfo(const QJsonObject &courseJsonObject) override;
    int courseid() const;
private:
    QLabel *mTitleLabel = nullptr;
    QLabel *mDetailLabel = nullptr;
    QLabel *mRatingAvgLabel = nullptr;
    QLabel *mRatingCountLabel = nullptr;

private:
    int mCourseid;
};

#endif // COURSEITEM_H
