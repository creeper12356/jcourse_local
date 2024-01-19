#include "coursecodelistwidget.h"
#include "coursecodeitem.h"

CourseCodeListWidget::CourseCodeListWidget(QWidget *parent)
    : AbstractListWidget(parent)
{

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

}
