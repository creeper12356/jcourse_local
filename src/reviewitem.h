#ifndef REVIEWITEM_H
#define REVIEWITEM_H

#include "inc.h"

class ReviewItem : public QListWidgetItem
{
    //评价条目
public:
    ReviewItem();
    virtual ~ReviewItem();
public:
    void updateReviewInfo(const QJsonObject &reviewJsonObject);
    //将条目添加到列表list
    void addToList(QListWidget* list);
    int reviewid() const;
private:
    QLabel *mRatingLabel = nullptr;
    QLabel *mSemesterLabel = nullptr;
    QTextBrowser* mCommentBrowser = nullptr;
    QLabel *mReactionLabel = nullptr;
    QWidget *mWidget = nullptr;
private:
    int mReviewid;
};

#endif // REVIEWITEM_H
