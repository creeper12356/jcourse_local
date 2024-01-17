#include "reviewlistwidget.h"

ReviewListWidget::ReviewListWidget(QWidget *parent)
    : AbstractListWidget(parent)
{
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setSelectionMode(QAbstractItemView::NoSelection);
}

void ReviewListWidget::resizeEvent(QResizeEvent *e)
{
    AbstractListWidget::resizeEvent(e);
}
