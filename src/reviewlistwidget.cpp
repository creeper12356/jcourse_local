#include "reviewlistwidget.h"
#include "reviewitem.h"
#include "paginationwidget.h"

ReviewListWidget::ReviewListWidget(QWidget *parent)
    : AbstractListWidget(parent)
{
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setSelectionMode(QAbstractItemView::NoSelection);
}

void ReviewListWidget::displayResult(const QJsonObject& resultJsonObject)
{
    this->clear();
    QJsonArray resultsJsonArray = resultJsonObject["results"].toArray();

    for(auto it = resultsJsonArray.begin();it != resultsJsonArray.end(); ++it){
        ReviewItem *newItem = new ReviewItem;
        newItem->updateItemInfo((*it).toObject());
        addItem(newItem);
    }
    if(paginationWidget()){
        paginationWidget()->setCount(PaginationWidget::divideTotal(resultJsonObject["count"].toInt(),PAGE_SIZE));
    }
}
void ReviewListWidget::resizeEvent(QResizeEvent *e)
{
    AbstractListWidget::resizeEvent(e);
}

void ReviewListWidget::itemClickedSlot(QListWidgetItem *item)
{
    //do nothing
}
