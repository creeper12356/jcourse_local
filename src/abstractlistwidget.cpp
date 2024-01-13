#include "abstractlistwidget.h"

AbstractListWidget::AbstractListWidget(QWidget *parent)
    : QListWidget(parent)
{
    //信号与虚槽函数连接，实现多态
    connect(this,&AbstractListWidget::itemClicked,this,&AbstractListWidget::itemClickedSlot);
}

void AbstractListWidget::itemClickedSlot(QListWidgetItem *item)
{
    //default : ignore
}
