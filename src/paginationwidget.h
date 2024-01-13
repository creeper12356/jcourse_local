#ifndef PAGINATIONWIDGET_H
#define PAGINATIONWIDGET_H

#include "inc.h"

class PaginationWidget : public QWidget
{
    //分页控件
    Q_OBJECT
public:
    explicit PaginationWidget(QWidget *parent = nullptr);
    ~PaginationWidget();

public:
    //返回mCount字段是否被成功修改
    bool setCount(int count);
    //返回mCurrent字段是否被成功修改
    bool setCurrent(int current);
    //根据给定的item总数和每页item数 ， 计算需要的总页数，通常与setCurrent一起使用
    static int divideTotal(int itemCount , int itemCountPerPage);
    int count() const;
    int current() const;
signals:
    //由于用户的点击
    //页面发生变化
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
