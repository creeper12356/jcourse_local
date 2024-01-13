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
    QToolBar* toolBar = new QToolBar(this);
    toolBar->addWidget(ui->user_head);
    this->addToolBar(Qt::TopToolBarArea,toolBar);

    connect(ui->search_button,&QPushButton::clicked,this,&MainWindow::searchBarTriggered);
    connect(ui->search_edit,&QLineEdit::returnPressed,this,&MainWindow::searchBarTriggered);
    connect(ui->pagination_widget,&PaginationWidget::currentPageChanged,this,&MainWindow::searchPageTriggered);
    connect(ui->pagination_widget_2,&PaginationWidget::currentPageChanged,this,&MainWindow::checkReviewPageTriggered);

    connect(ui->course_item_list,&CourseListWidget::courseSelected,this,&MainWindow::checkReviewTriggered);
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
    if(ui->search_edit->text().isEmpty()){
        //不允许空白检索
        qDebug() << "empty query are not allowed";
        return ;
    }

    ui->pagination_widget->setCurrent(1);
    //保存请求
    mLastQuery = ui->search_edit->text();

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
    ui->pagination_widget->setCurrent(page);
    emit search(mLastQuery,page);
}

void MainWindow::checkReviewTriggered(int courseid)
{
    ui->pagination_widget_2->setCurrent(1);
    //保存请求
    mLastCourseid = courseid;
    emit checkReview(courseid,1);
}

void MainWindow::checkReviewPageTriggered(int page)
{
    ui->pagination_widget_2->setCurrent(page);
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
        newItem->updateCourseInfo((*it).toObject());
        newItem->addToList(ui->course_item_list);
    }
    ui->pagination_widget->setCount(resultJsonObject["count"].toInt() / PAGE_SIZE + 1);
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
        newItem->updateReviewInfo((*it).toObject());
        newItem->addToList(ui->review_item_list);
    }
    ui->pagination_widget_2->setCount(resultJsonObject["count"].toInt() / PAGE_SIZE + 1);
}

void MainWindow::userNameChangedSlot(QString userName)
{
    ui->user_head->setUserName(userName);
}
