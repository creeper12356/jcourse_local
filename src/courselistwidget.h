#ifndef COURSELISTWIDGET_H
#define COURSELISTWIDGET_H
#include "inc.h"
#include "abstractlistwidget.h"
class CourseListWidget: public AbstractListWidget
{
    Q_OBJECT
    //课程列表控件
public:
    explicit CourseListWidget(QWidget* parent = 0);
private slots:
    virtual void itemClickedSlot(QListWidgetItem* item) override;
signals:
//    课程被选中的信号，发给MainWindow
    void courseSelected(int courseid);
};

#endif // COURSELISTWIDGET_H
