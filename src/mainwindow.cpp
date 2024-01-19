#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent)
    ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //search_tab
    ui->search_edit->setPlaceholderText(
                "搜索课程名/课号/教师姓名/教师姓名拼音"
                );
    ui->search_edit->setFocus();

    ui->user_head->setMinimumSize(50,50);
    ui->top_layout->setAlignment(Qt::AlignLeft);

    ui->course_item_list->setPaginationWidget(ui->course_page_widget);
    ui->review_item_list->setPaginationWidget(ui->review_page_widget);

    ui->switch_button->setFixedSize(70,30);
    ui->switch_button->setTextOn("在线");
    ui->switch_button->setTextOff("离线");
    connect(ui->switch_button,&SwitchButton::statusChanged,this,&MainWindow::changeOnline);

    ui->course_info_label->setAlignment(Qt::AlignCenter);
    ui->review_info_label->setAlignment(Qt::AlignCenter);

    connect(ui->search_button,&QPushButton::clicked,this,&MainWindow::searchBarTriggered);
    connect(ui->search_edit,&QLineEdit::returnPressed,this,&MainWindow::searchBarTriggered);
    connect(ui->course_page_widget,&PaginationWidget::currentPageChanged,this,&MainWindow::searchPageTriggered);
    connect(ui->course_item_list,&CourseListWidget::courseSelected,this,&MainWindow::checkReviewTriggered);
    connect(ui->review_page_widget,&PaginationWidget::currentPageChanged,this,&MainWindow::checkReviewPageTriggered);
    connect(ui->logout_button,&QPushButton::clicked,this,&MainWindow::logout);

    //download_tab
    ui->parse_button->setMaximumWidth(100);
    connect(ui->parse_button,&QPushButton::clicked,this,&MainWindow::parseCourseStatusTriggered);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::searchBarTriggered()
{
    QString query = ui->search_edit->text();
    query = query.trimmed();
    if(query.isEmpty()){
        //不允许空白检索
        ui->statusbar->showMessage("请输入搜索内容");
        return ;
    }

    ui->course_page_widget->setCurrent(1);
    //保存请求
    mLastQuery = query;

    ui->search_edit->clear();
    ui->statusbar->showMessage(SEARCH_MESSAGE(mLastQuery,1));
    emit search(mLastQuery,1);
}

void MainWindow::searchPageTriggered(int page)
{
    if(mLastQuery.isEmpty()){
        //不允许空白检索
        qDebug() << "empty query are not allowed";
        return ;
    }
    ui->statusbar->showMessage(SEARCH_MESSAGE(mLastQuery,page));
    emit search(mLastQuery,page);
}

void MainWindow::checkReviewTriggered(int courseid)
{
    //after requesting new query , go back to first page
    ui->review_page_widget->setCurrent(1);
    //保存请求
    mLastCourseid = courseid;
    ui->statusbar->showMessage(REVIEW_MESSAGE(courseid,1));
    emit checkReview(courseid,1);
}

void MainWindow::checkReviewPageTriggered(int page)
{
    ui->statusbar->showMessage(REVIEW_MESSAGE(mLastCourseid,page));
    emit checkReview(mLastCourseid,page);
}

void MainWindow::parseCourseStatusTriggered()
{
    QString plainText = ui->course_status_edit->toPlainText();
    if(plainText.isEmpty()){
        qDebug() << "course status source cannot be empty.";
        return ;
    }
    emit parseCourseStatus(ui->course_status_edit->toPlainText());
}

void MainWindow::displaySearchResult(QByteArray result)
{
    ui->statusbar->clearMessage();
    QJsonObject resultJsonObject = QJsonDocument::fromJson(result).object();
    ui->course_info_label->setText(COURSE_INFO(resultJsonObject["count"].toInt()));
    ui->review_info_label->setText("请选择一门课");
    ui->course_item_list->displayResult(resultJsonObject);
}

void MainWindow::displayCheckReviewResult(QByteArray result)
{
    ui->statusbar->clearMessage();
    QJsonObject resultJsonObject = QJsonDocument::fromJson(result).object();
    ui->review_info_label->setText(REVIEW_INFO(resultJsonObject["count"].toInt()));
    ui->review_item_list->displayResult(resultJsonObject);
}

void MainWindow::displayParseCourseStatusResult(QJsonObject resultJsonObject)
{
    ui->download_course_list_widget->displayResult(resultJsonObject);
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
