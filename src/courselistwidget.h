#ifndef COURSELISTWIDGET_H
#define COURSELISTWIDGET_H
#include "inc.h"

class CourseListWidget: public QListWidget
{
    Q_OBJECT
    //课程列表控件
public:
    explicit CourseListWidget(QWidget* parent = 0);
private slots:
    void itemClickedSlot(QListWidgetItem* item);
signals:
    //课程被选中的信号，发给MainWindow
    void courseSelected(int courseid);
};

#endif // COURSELISTWIDGET_H
