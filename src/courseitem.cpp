#include "courseitem.h"

CourseItem::CourseItem()
    :AbstractItem()
{
    mTitleLabel = new QLabel(nullptr);
    mDetailLabel = new QLabel(nullptr);
    mRatingAvgLabel = new QLabel(nullptr);
    mRatingCountLabel = new QLabel(nullptr);

    mTitleLabel->setAlignment(Qt::AlignLeft);
    mDetailLabel->setAlignment(Qt::AlignLeft);
    mRatingAvgLabel->setAlignment(Qt::AlignRight);
    mRatingCountLabel->setAlignment(Qt::AlignRight);

    QHBoxLayout* topLayout = new QHBoxLayout(nullptr);
    topLayout->addWidget(mTitleLabel,1);
    topLayout->addWidget(mRatingAvgLabel,1);

    QHBoxLayout* bottomLayout = new QHBoxLayout(nullptr);
    bottomLayout->addWidget(mDetailLabel,1);
    bottomLayout->addWidget(mRatingCountLabel,1);

    QVBoxLayout* layout = new QVBoxLayout(widget());
    layout->addLayout(topLayout,1);
    layout->addLayout(bottomLayout,1);

    widget()->setLayout(layout);
    setSizeHint(QSize(sizeHint().width(),70));
}

CourseItem::~CourseItem()
{
}

void CourseItem::updateItemInfo(const QJsonObject &courseJsonObject)
{
    //暂存课程id
    mCourseid = courseJsonObject["id"].toInt();
//    qDebug() << "id: " << mCourseid;

    //修改显示内容
    QString title = "%1 %2 (%3)";
    title = title.arg(
            courseJsonObject["code"].toString(),
            courseJsonObject["name"].toString(),
            courseJsonObject["teacher"].toString());
    mTitleLabel->setText(title);

    QString detail = "%1学分 %2";
    detail = detail.arg(
             QString::number(courseJsonObject["credit"].toInt()),
             courseJsonObject["department"].toString());
    mDetailLabel->setText(detail);


    if(courseJsonObject["rating"].toObject()["count"].toInt()){
        mRatingAvgLabel->setText(QString::number(courseJsonObject["rating"].toObject()["avg"].toDouble(),'f',1));
        QString ratingCount = "%1人评价";
        ratingCount = ratingCount.arg(courseJsonObject["rating"].toObject()["count"].toInt());
        mRatingCountLabel->setText(ratingCount);
    }
    else{
        mRatingAvgLabel->setText("暂无点评");
        mRatingCountLabel->clear();
    }
}

int CourseItem::courseid() const
{
    return mCourseid;
}
