#ifndef ABSTRACTLISTWIDGET_H
#define ABSTRACTLISTWIDGET_H

#include "inc.h"

/*!
 * \brief 列表控件抽象类
 *
 * Inherits: QListWidget
 *
 * Inherited by: CourseListWidget, ReviewListWidget
 *
 * 列表控件抽象类，支持连接分页控件使用。
 */
class AbstractListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AbstractListWidget(QWidget* parent = 0);

    /*!
     * \brief 更新列表视图
     *
     * 解析QJsonObject对象，并显示到界面上，该方法需要派生类实现。
     * \param resultJsonObject 待解析的QJsonObject的引用
     */
    virtual void displayResult(const QJsonObject& resultJsonObject) = 0;

    void addItem(AbstractItem* item);
    /*!
     * \brief 移除并回收所有条目
     */
    void clear();

    /*!
     * \brief 设置分页控件
     * \param widget 分页控件的指针
     */
    void setPaginationWidget(PaginationWidget* widget);

    /*!
     * \brief 返回分页控件的指针
     */
    PaginationWidget* paginationWidget();

protected slots:

    /*!
     * \brief 条目被点击的槽函数
     *
     * 当列表中的条目被点击时，通过此槽函数作出响应，该方法需要派生类实现。
     * \param item 指向被点击条目的指针
     */
    virtual void itemClickedSlot(QListWidgetItem *item) = 0;

private:
    PaginationWidget* mPaginationWidget = nullptr;
};

#endif // ABSTRACTLISTWIDGET_H
