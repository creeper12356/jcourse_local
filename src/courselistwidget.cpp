#include "courselistwidget.h"
#include "courseitem.h"
#include "paginationwidget.h"
CourseListWidget::CourseListWidget(QWidget *parent)
    : AbstractListWidget(parent)
{
    mCacheAction = new QAction("download all review");
    mContextMenu->addAction(mCacheAction);

}

CourseListWidget::~CourseListWidget()
{
    delete mCacheAction;
}

void CourseListWidget::itemClickedSlot(QListWidgetItem *item)
{
    int courseid = dynamic_cast<CourseItem*>(item)->courseid();
    qDebug() << courseid;
    emit courseSelected(courseid);
}

void CourseListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if(itemAt(event->pos())){
        QAction* triggered = mContextMenu->exec(event->globalPos());
        if(triggered == mCacheAction){
            CourseItem* targetItem = dynamic_cast<CourseItem*>(itemAt(event->pos()));
            emit cacheCourseReview(targetItem->courseid());
        }
    }
}

void CourseListWidget::displayResult(const QJsonObject &resultJsonObject)
{
    this->clear();
    QJsonArray resultJsonArray = resultJsonObject["results"].toArray();
    //添加新条目
    for(auto it = resultJsonArray.begin();it != resultJsonArray.end(); ++it){
        CourseItem *newItem = new CourseItem;
        newItem->updateItemInfo((*it).toObject());
        addItem(newItem);
    }
    if(paginationWidget()){
        paginationWidget()->setCount(PaginationWidget::divideTotal(resultJsonObject["count"].toInt(),PAGE_SIZE));
    }
}
