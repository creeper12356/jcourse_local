#include "reviewlistwidget.h"

ReviewListWidget::ReviewListWidget(QWidget *parent)
    : AbstractListWidget(parent)
{
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}
