#include "coursecodeitem.h"

CourseCodeItem::CourseCodeItem()
    : AbstractItem()
{
    mCodeLabel = new QLabel;
    mNameLabel = new QLabel;
    mSemesterLabel = new QLabel;
    mCreditLabel = new QLabel;

    QHBoxLayout* layout = new QHBoxLayout(widget());
    layout->addWidget(mCodeLabel);
    layout->addWidget(mNameLabel);
    layout->addWidget(mSemesterLabel);
}

CourseCodeItem::~CourseCodeItem()
{

}

void CourseCodeItem::updateItemInfo(const QJsonObject &jsonObject)
{
    mCodeLabel->setText(jsonObject["code"].toString());
    mNameLabel->setText(jsonObject["name"].toString());
    mSemesterLabel->setText(jsonObject["semester"].toString());
    mCreditLabel->setText(QString::number(jsonObject["credit"].toDouble()));
}

const QString &CourseCodeItem::courseCode() const
{
    return mCourseCode;
}
