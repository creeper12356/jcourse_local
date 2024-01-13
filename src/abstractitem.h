#ifndef ABSTRACTITEM_H
#define ABSTRACTITEM_H
#include "inc.h"
//统一 CourseItem和ReviewItem
class AbstractItem : public QListWidgetItem
{
    //widget item抽象类
public:
    AbstractItem();
    virtual ~AbstractItem();
public:
    //根据JSON对象对应字段，更新显示的信息
    virtual void updateItemInfo(const QJsonObject& jsonObject) = 0;
    //将条目添加到列表list
    void addToList(QListWidget *list);

protected:
    //派生类访问mWidget接口
    QWidget* widget() ;

private:
    //显示控件
    QWidget* mWidget = nullptr;
};

#endif // ABSTRACTITEM_H
