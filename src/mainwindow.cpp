#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "courseitem.h"

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

    connect(ui->search_button,&QPushButton::clicked,this,&MainWindow::searchTriggered);
    connect(ui->search_edit,&QLineEdit::returnPressed,this,&MainWindow::searchTriggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::searchTriggered()
{
    emit search(ui->search_edit->text());
    ui->search_edit->clear();
}

void MainWindow::cacheChangedSlot(QString cache)
{
//    ui->result_browser->setText(cache);
    qDebug() << cache;
    QJsonObject cacheJsonObject = QJsonDocument::fromJson(cache.toUtf8()).object();
    QJsonArray resultsJsonArray = cacheJsonObject["results"].toArray();
    while(ui->course_item_list->count() > 0){
        delete ui->course_item_list->takeItem(0);
    }

    for(auto it = resultsJsonArray.begin();it != resultsJsonArray.end(); ++it){
        CourseItem *newItem = new CourseItem;
        newItem->updateCourseInfo((*it).toObject());
        newItem->addToList(ui->course_item_list);
    }
}

void MainWindow::userNameChangedSlot(QString userName)
{
    ui->user_head->setUserName(userName);
}


