#include "reviewitem.h"
ReviewItem::ReviewItem()
    : QListWidgetItem(nullptr)
{
    mWidget = new QWidget(nullptr);
    mRatingLabel = new QLabel(nullptr);
    mSemesterLabel = new QLabel(nullptr);
    mCommentBrowser = new QTextBrowser(nullptr);
    mReactionLabel = new QLabel(nullptr);

    QHBoxLayout* topLayout = new QHBoxLayout(nullptr);
    topLayout->setAlignment(Qt::AlignLeft);
    topLayout->addWidget(mRatingLabel);
    topLayout->addWidget(mSemesterLabel);

    QVBoxLayout* layout = new QVBoxLayout(mWidget);
    layout->addLayout(topLayout,0);
    layout->addWidget(mCommentBrowser,1);
    layout->addWidget(mReactionLabel,0);

    mWidget->setLayout(layout);
    //TODO : 调整item高度（自适应）
    this->setSizeHint(QSize(sizeHint().width(),600));

}

ReviewItem::~ReviewItem()
{
    delete mWidget;
}

void ReviewItem::updateReviewInfo(const QJsonObject &reviewJsonObject)
{
    //暂存评价id
    mReviewid = reviewJsonObject["id"].toInt();

    //修改显示内容
    mRatingLabel->setText("推荐指数：" + QString::number(reviewJsonObject["rating"].toInt()));
    mSemesterLabel->setText("学期：" + reviewJsonObject["semester"].toString());
    mCommentBrowser->setText(reviewJsonObject["comment"].toString());
    int approves = reviewJsonObject["reactions"].toObject()["approves"].toInt();
    int disapproves = reviewJsonObject["reactions"].toObject()["disapproves"].toInt();
    qDebug() << disapproves;

    mReactionLabel->setText(QString("∆%1   ∇%2").arg(QString::number(approves),
                                                              QString::number(disapproves)));
}

void ReviewItem::addToList(QListWidget *list)
{
    list->addItem(this);
    list->setItemWidget(this,mWidget);
}

int ReviewItem::reviewid() const
{
    return mReviewid;
}
