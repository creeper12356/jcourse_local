#include "abstractlistwidget.h"
#include "abstractitem.h"
AbstractListWidget::AbstractListWidget(QWidget *parent)
    : QListWidget(parent)
{
    //信号与虚槽函数连接，实现多态
    connect(this,&AbstractListWidget::itemClicked,this,&AbstractListWidget::itemClickedSlot);
}

void AbstractListWidget::addItem(AbstractItem *item)
{
    QListWidget::addItem(item);
    QListWidget::setItemWidget(item,item->widget());

}

void AbstractListWidget::clear()
{
   while(count() > 0){
        delete takeItem(0);
   }
}

void AbstractListWidget::setPaginationWidget(PaginationWidget *widget)
{
    mPaginationWidget = widget;
}

PaginationWidget *AbstractListWidget::paginationWidget()
{
    return mPaginationWidget;
}
