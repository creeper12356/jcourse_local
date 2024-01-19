#ifndef COURSECODEITEM_H
#define COURSECODEITEM_H
#include "inc.h"
#include "abstractitem.h"
class CourseCodeItem : public AbstractItem
{
public:
    CourseCodeItem();
    virtual ~CourseCodeItem();
public:
    virtual void updateItemInfo(const QJsonObject &jsonObject) override;
    const QString &courseCode() const;

private:
    QLabel* mCodeLabel = nullptr;
    QLabel* mNameLabel = nullptr;
    QLabel* mSemesterLabel = nullptr;
    QLabel* mCreditLabel = nullptr;
private:
    QString mCourseCode;
};

#endif // COURSECODEITEM_H
