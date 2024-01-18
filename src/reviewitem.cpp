#include "reviewitem.h"
ReviewItem::ReviewItem()
    : AbstractItem()
{
    mRatingLabel = new QLabel(nullptr);
    mSemesterLabel = new QLabel(nullptr);
    mScoreLabel = new QLabel(nullptr);
    mCommentBrowser = new QTextBrowser(nullptr);
    mTimeLabel = new QLabel(nullptr);
    mReactionLabel = new QLabel(nullptr);

    QHBoxLayout* topLayout = new QHBoxLayout(nullptr);
    topLayout->setAlignment(Qt::AlignLeft);
    topLayout->addWidget(mRatingLabel);
    topLayout->addWidget(mSemesterLabel);
    topLayout->addWidget(mScoreLabel);

    QVBoxLayout* layout = new QVBoxLayout(widget());
    layout->addLayout(topLayout,0);
    layout->addWidget(mCommentBrowser,1);
    layout->addWidget(mTimeLabel,0);
    layout->addWidget(mReactionLabel,0);

    widget()->setLayout(layout);

    //set fixed item height
    setSizeHint(QSize(sizeHint().width(),400));
}

ReviewItem::~ReviewItem()
{
}

void ReviewItem::updateItemInfo(const QJsonObject &reviewJsonObject)
{
    //暂存评价id
    mReviewid = reviewJsonObject["id"].toInt();

    //修改显示内容
    mRatingLabel->setText("推荐指数：" + QString::number(reviewJsonObject["rating"].toInt()));
    mSemesterLabel->setText("学期：" + reviewJsonObject["semester"].toString());
    if(reviewJsonObject["score"].isString() && !reviewJsonObject["score"].toString().isEmpty()){
        //成绩不为空
        mScoreLabel->setText("成绩：" + reviewJsonObject["score"].toString());
    }
    else{
        mScoreLabel->clear();
    }

    mCommentBrowser->setText(reviewJsonObject["comment"].toString());

    if(reviewJsonObject["created_at"] == reviewJsonObject["modified_at"]){
        //首次发表
        mTimeLabel->setText("发表于 " + reviewJsonObject["created_at"].toString());
    }
    else{
        //编辑
        mTimeLabel->setText("编辑于 " + reviewJsonObject["modified_at"].toString());
    }

    int approves = reviewJsonObject["reactions"].toObject()["approves"].toInt();
    int disapproves = reviewJsonObject["reactions"].toObject()["disapproves"].toInt();

    mReactionLabel->setText(QString("∆%1   ∇%2").arg(QString::number(approves),
                                                              QString::number(disapproves)));
}

int ReviewItem::reviewid() const
{
    return mReviewid;
}
