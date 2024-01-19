#ifndef COURSECODELISTWIDGET_H
#define COURSECODELISTWIDGET_H
#include "inc.h"
#include "abstractlistwidget.h"

class CourseCodeListWidget : public AbstractListWidget
{
    Q_OBJECT
public:
    explicit CourseCodeListWidget(QWidget* parent = 0);
    void displayResult(const QJsonObject& resultJsonObject) override;
protected:
    void itemClickedSlot(QListWidgetItem *item) override;
};

#endif // COURSECODELISTWIDGET_H
