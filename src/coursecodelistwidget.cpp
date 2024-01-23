#include "coursecodelistwidget.h"
#include "coursecodeitem.h"

CourseCodeListWidget::CourseCodeListWidget(QWidget *parent)
    : AbstractListWidget(parent)
{
    mCacheAction = new QAction("缓存这门课");
    mContextMenu->addAction(mCacheAction);
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
    qDebug() << this->itemAt(event->pos());
    mContextMenu->exec(event->globalPos());
}
