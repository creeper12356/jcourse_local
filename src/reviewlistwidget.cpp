#include "reviewlistwidget.h"

ReviewListWidget::ReviewListWidget(QWidget *parent)
    :QListWidget(parent)
{
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}
