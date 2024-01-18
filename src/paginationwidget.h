#ifndef PAGINATIONWIDGET_H
#define PAGINATIONWIDGET_H

#include "inc.h"

/*!
 * \brief 分页控件
 */
class PaginationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaginationWidget(QWidget *parent = nullptr);
    ~PaginationWidget();

public:

    /*!
     * \brief 设置总页数
     * \param count
     * \return 总页数是否发生变化
     */
    bool setCount(int count);

    /*!
     * \brief 设置当前页数
     * \param current
     * \return 当前页数是否发生变化
     */
    bool setCurrent(int current);

    /*!
     * \sa setCurrent
     * \brief 计算需要的总页数
     *
     * 根据给定的条目总数和每页的条目数，计算需要的总页数，通常与setCurrent函数一起使用。
     * \param itemCount 条目总数
     * \param itemCountPerPage 每页的条目数
     * \return 需要的总页数
     */
    static int divideTotal(int itemCount , int itemCountPerPage);

    int count() const;//!<总页数

    int current() const;//!<当前页数
signals:

    /*!
     * \brief 当前页数改变的信号
     * \param page 当前页数
     */
    void currentPageChanged(int page);

/* === above visible to user === */

private:
    //设置mCurrent，如果修改成功发送currentPageChanged信号
    void setCurrentUpdate(int current);
private slots:
    void firstPageClicked();
    void prevPageClicked();
    void nextPageClicked();
    void lastPageClicked();
private:
    /* 第一页，上一页，下一页，最后一页 */
    QPushButton* mFirstButton = nullptr;
    QPushButton* mPrevButton = nullptr;
    QPushButton* mNextButton = nullptr;
    QPushButton* mLastButton = nullptr;

    //currentPage/totalPage
    QLabel* mPageLabel = nullptr;
private:
    int mCount;
    //当前页面，有效范围[1,mCount]
    int mCurrent;

};

#endif // PAGINATIONWIDGET_H
