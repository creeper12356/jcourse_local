#include "mainwindow.h"
#include <QHBoxLayout>
#include <QLineEdit>
MainWindow::MainWindow(QWidget *parent)
{
    QLineEdit* apiLineEdit = new QLineEdit(this);
    connect(apiLineEdit,&QLineEdit::returnPressed,this,[this,apiLineEdit](){
       emit search(apiLineEdit->text());
       apiLineEdit->clear();
    });
}

MainWindow::~MainWindow()
{
}


