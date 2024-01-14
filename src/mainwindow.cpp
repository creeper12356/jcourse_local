#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "courseitem.h"
#include "reviewitem.h"

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent)
    ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->search_edit->setPlaceholderText(
                "搜索课程名/课号/教师姓名/教师姓名拼音"
                );

    ui->user_head->setMinimumSize(50,50);
    ui->top_layout->setAlignment(Qt::AlignLeft);

    ui->switch_button->setFixedSize(70,30);
    ui->switch_button->setTextOn("在线");
    ui->switch_button->setTextOff("离线");
    connect(ui->switch_button,&SwitchButton::statusChanged,this,&MainWindow::changeOnline);
//    QToolBar* toolBar = new QToolBar(this);
//    toolBar->addWidget(ui->user_head);
//    this->addToolBar(Qt::TopToolBarArea,toolBar);

    connect(ui->search_button,&QPushButton::clicked,this,&MainWindow::searchBarTriggered);
    connect(ui->search_edit,&QLineEdit::returnPressed,this,&MainWindow::searchBarTriggered);
    connect(ui->course_page_widget,&PaginationWidget::currentPageChanged,this,&MainWindow::searchPageTriggered);
    connect(ui->review_page_widget,&PaginationWidget::currentPageChanged,this,&MainWindow::checkReviewPageTriggered);

    connect(ui->course_item_list,&CourseListWidget::courseSelected,this,&MainWindow::checkReviewTriggered);

    connect(ui->logout_button,&QPushButton::clicked,this,&MainWindow::logout);
}

MainWindow::~MainWindow()
{
    qDebug() << "delete items";
    while(ui->course_item_list->count() > 0){
        delete ui->course_item_list->takeItem(0);
    }
    while(ui->review_item_list->count() > 0){
        delete ui->review_item_list->takeItem(0);
    }

    qDebug() << "delete ui";
    delete ui;
}

void MainWindow::searchBarTriggered()
{
    QString query = ui->search_edit->text();
    query = query.trimmed();
    if(query.isEmpty()){
        //不允许空白检索
        qDebug() << "empty query are not allowed";
        return ;
    }

    ui->course_page_widget->setCurrent(1);
    //保存请求
    mLastQuery = query;

    ui->search_edit->clear();
    emit search(mLastQuery,1);
}

void MainWindow::searchPageTriggered(int page)
{
    if(mLastQuery.isEmpty()){
        //不允许空白检索
        qDebug() << "empty query are not allowed";
        return ;
    }
    emit search(mLastQuery,page);
}

void MainWindow::checkReviewTriggered(int courseid)
{
    //after requesting new query , go back to first page
    ui->review_page_widget->setCurrent(1);
    //保存请求
    mLastCourseid = courseid;
    emit checkReview(courseid,1);
}

void MainWindow::checkReviewPageTriggered(int page)
{
    emit checkReview(mLastCourseid,page);
}

void MainWindow::displaySearchResult(QByteArray result)
{
    QJsonObject resultJsonObject = QJsonDocument::fromJson(result).object();
    QJsonArray resultsJsonArray = resultJsonObject["results"].toArray();
    while(ui->course_item_list->count() > 0){
        delete ui->course_item_list->takeItem(0);
    }

    for(auto it = resultsJsonArray.begin();it != resultsJsonArray.end(); ++it){
        CourseItem *newItem = new CourseItem;
        newItem->updateItemInfo((*it).toObject());
        newItem->addToList(ui->course_item_list);
    }
    ui->course_page_widget->setCount(PaginationWidget::divideTotal(resultJsonObject["count"].toInt(),PAGE_SIZE));
}

void MainWindow::displayCheckReviewResult(QByteArray result)
{
    //TODO : 合并courseListWidget 和 reviewListWidget

    QJsonObject resultJsonObject = QJsonDocument::fromJson(result).object();
    QJsonArray resultsJsonArray = resultJsonObject["results"].toArray();
    while(ui->review_item_list->count() > 0){
        delete ui->review_item_list->takeItem(0);
    }

    for(auto it = resultsJsonArray.begin();it != resultsJsonArray.end(); ++it){
        ReviewItem *newItem = new ReviewItem;
        newItem->updateItemInfo((*it).toObject());
        newItem->addToList(ui->review_item_list);
    }
    ui->review_page_widget->setCount(PaginationWidget::divideTotal(resultJsonObject["count"].toInt(),PAGE_SIZE));
}

void MainWindow::userNameChangedSlot(QString userName)
{
    ui->user_head->setUserName(userName);
    ui->user_label->setText(userName);
}

void MainWindow::onlineChangedSlot(bool isOnline)
{
    ui->switch_button->setChecked(isOnline);
}
