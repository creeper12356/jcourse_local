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

void AbstractItem::addToList(QListWidget *list)
{
    list->addItem(this);
    list->setItemWidget(this,mWidget);
}

QWidget *AbstractItem::widget()
{
    return mWidget;
}
