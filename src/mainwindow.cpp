#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent)
    ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->search_edit->setPlaceholderText(
                "搜索课程名/课号/教师姓名/教师姓名拼音"
                );
    connect(ui->search_button,&QPushButton::clicked,this,&MainWindow::searchTriggered);
    connect(ui->search_edit,&QLineEdit::returnPressed,this,&MainWindow::searchTriggered);
}

MainWindow::~MainWindow()
{
}

void MainWindow::searchTriggered()
{
    emit search(ui->search_edit->text());
    ui->search_edit->clear();
}

void MainWindow::cacheChangedSlot(QString cache)
{
    ui->result_browser->setText(cache);
}


