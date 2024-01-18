#ifndef ABSTRACTITEM_H
#define ABSTRACTITEM_H
#include "inc.h"

/*!
 * \brief 列表条目抽象类
 *
 * Inherits: QListWidgetItem
 *
 * Inherited by: CourseItem, ReviewItem
 */
class AbstractItem : public QListWidgetItem
{
public:
    AbstractItem();
    virtual ~AbstractItem();
public:

    /*!
     * \brief 更新条目显示的信息
     *
     * 解析QJsonObject对象，并显示到界面上。
     * \param jsonObject 待解析的QJsonObject对象
     */
    virtual void updateItemInfo(const QJsonObject& jsonObject) = 0;

    /*!
     * \brief 显示窗口部件
     */
    QWidget* widget() ;

private:
    QWidget* mWidget = nullptr; //!<显示窗口部件
};

#endif // ABSTRACTITEM_H
