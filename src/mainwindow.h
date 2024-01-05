#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "inc.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void search(QString query);
};
#endif // MAINWINDOW_H
