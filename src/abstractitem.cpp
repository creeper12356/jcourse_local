#include "abstractitem.h"

AbstractItem::AbstractItem()
    :QListWidgetItem(nullptr)
{
    mWidget = new QWidget(nullptr);
}

AbstractItem::~AbstractItem()
{
    delete mWidget;
}

QWidget *AbstractItem::widget()
{
    return mWidget;
}
