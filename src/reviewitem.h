#ifndef REVIEWITEM_H
#define REVIEWITEM_H

#include "inc.h"
#include "abstractitem.h"

/*!
 * \brief 课程评价条目
 *
 * Inherits: AbstractItem
 */
class ReviewItem : public AbstractItem
{
public:
    ReviewItem();
    virtual ~ReviewItem();
public:
    virtual void updateItemInfo(const QJsonObject &reviewJsonObject) override;
    //将条目添加到列表list
    int reviewid() const;

private:
    QLabel *mRatingLabel = nullptr;
    QLabel *mSemesterLabel = nullptr;
    QLabel *mScoreLabel = nullptr;
    QTextBrowser* mCommentBrowser = nullptr;
    QLabel *mTimeLabel = nullptr;
    QLabel *mReactionLabel = nullptr;
private:
    int mReviewid;
};

#endif // REVIEWITEM_H
