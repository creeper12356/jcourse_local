#include "paginationwidget.h"

PaginationWidget::PaginationWidget(QWidget *parent) : QWidget(parent)
{
    mFirstButton = new QPushButton("<<");
    mFirstButton->setFixedSize(20,20);
    mFirstButton->setToolTip("第一页");

    mPrevButton = new QPushButton("<");
    mPrevButton->setFixedSize(20,20);
    mPrevButton->setToolTip("上一页");

    mNextButton = new QPushButton(">");
    mNextButton->setFixedSize(20,20);
    mNextButton->setToolTip("下一页");

    mLastButton = new QPushButton(">>");
    mLastButton->setFixedSize(20,20);
    mLastButton->setToolTip("最后一页");

    mPageLabel = new QLabel;
    mPageLabel->setFixedHeight(20);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignRight);
    layout->addWidget(mFirstButton);
    layout->addWidget(mPrevButton);
    layout->addWidget(mPageLabel);
    layout->addWidget(mNextButton);
    layout->addWidget(mLastButton);

    connect(mFirstButton,&QPushButton::clicked,this,&PaginationWidget::firstPageClicked);
    connect(mPrevButton,&QPushButton::clicked,this,&PaginationWidget::prevPageClicked);
    connect(mNextButton,&QPushButton::clicked,this,&PaginationWidget::nextPageClicked);
    connect(mLastButton,&QPushButton::clicked,this,&PaginationWidget::lastPageClicked);
    this->setLayout(layout);
    //必须先初始化mCount，再初始化mCurrent
    setCount(1);
    setCurrent(1);
}

PaginationWidget::~PaginationWidget()
{
}

bool PaginationWidget::setCount(int count)
{
    if(count <= 0){
        qDebug() << "count cannot be negative.";
        return false;
    }
    //assert count >= 0
    mCount = count;
    mPageLabel->setText(QString::number(mCurrent) + "/" + QString::number(mCount));
    return true;
}

bool PaginationWidget::setCurrent(int current)
{
    if(current <= 0 || current > mCount){
        qDebug() << "invalid current index";
        return false;
    }
    if(current == mCurrent){
        //已经在当前页面
        return false;
    }
    //assert current is valid
    mCurrent = current;
    mPageLabel->setText(QString::number(mCurrent) + "/" + QString::number(mCount));
    return true;
}

int PaginationWidget::count() const
{
    return mCount;
}

int PaginationWidget::current() const
{
    return mCurrent;
}

void PaginationWidget::setCurrentUpdate(int current)
{
    if(setCurrent(current)){
        emit currentPageChanged(mCurrent);
    }
}


void PaginationWidget::firstPageClicked()
{
    setCurrentUpdate(1);
}

void PaginationWidget::prevPageClicked()
{
    setCurrentUpdate(mCurrent - 1);
}

void PaginationWidget::nextPageClicked()
{
    setCurrentUpdate(mCurrent + 1);
}

void PaginationWidget::lastPageClicked()
{
    setCurrentUpdate(mCount);
}
