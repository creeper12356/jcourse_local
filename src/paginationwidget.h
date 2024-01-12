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
    const int count() const;
    const int current() const;
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
