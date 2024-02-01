#include "coursecodelistwidget.h"
#include "coursecodeitem.h"

CourseCodeListWidget::CourseCodeListWidget(QWidget *parent)
    : AbstractListWidget(parent)
{
    mCacheAction = new QAction("缓存这门课",this);
    mContextMenu->addAction(mCacheAction);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

CourseCodeListWidget::~CourseCodeListWidget()
{
    delete mCacheAction;
}

void CourseCodeListWidget::displayResult(const QJsonObject &resultJsonObject)
{
    this->clear();
    QJsonArray resultJsonArray = resultJsonObject["results"].toArray();
    //添加新条目
    for(auto it = resultJsonArray.begin();it != resultJsonArray.end(); ++it){
        CourseCodeItem *newItem = new CourseCodeItem;
        newItem->updateItemInfo((*it).toObject());
        addItem(newItem);
    }
}

void CourseCodeListWidget::itemClickedSlot(QListWidgetItem *item)
{
    qDebug() << "do nothing.";
}

void CourseCodeListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if(itemAt(event->pos())){
        auto action = mContextMenu->exec(event->globalPos());
        if(action == mCacheAction){
            CourseCodeItem* targetItem = dynamic_cast<CourseCodeItem*> (itemAt(event->pos()));
            emit cacheCourseCodeReview(targetItem->courseCode());
        }
    }
}
