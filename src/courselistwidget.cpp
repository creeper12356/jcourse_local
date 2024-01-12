#include "courselistwidget.h"
#include "courseitem.h"
CourseListWidget::CourseListWidget(QWidget *parent)
    : QListWidget(parent)
{
    connect(this,&CourseListWidget::itemClicked,this,&CourseListWidget::itemClickedSlot);
}

void CourseListWidget::itemClickedSlot(QListWidgetItem *item)
{
    int courseid = dynamic_cast<CourseItem*>(item)->courseid();
    qDebug() << courseid;
    emit courseSelected(courseid);
}
