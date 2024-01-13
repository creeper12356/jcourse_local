#include "courselistwidget.h"
#include "courseitem.h"
CourseListWidget::CourseListWidget(QWidget *parent)
    : AbstractListWidget(parent)
{
}

void CourseListWidget::itemClickedSlot(QListWidgetItem *item)
{
    int courseid = dynamic_cast<CourseItem*>(item)->courseid();
    qDebug() << courseid;
    emit courseSelected(courseid);
}
