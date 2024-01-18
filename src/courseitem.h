#ifndef COURSEITEM_H
#define COURSEITEM_H

#include "inc.h"
#include "abstractitem.h"

/*!
 * \brief 课程条目
 *
 * Inherits: AbstractItem
 */
class CourseItem : public AbstractItem
{
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
