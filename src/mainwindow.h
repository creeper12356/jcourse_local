#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "inc.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //搜索框被触发
    void searchBarTriggered();
    //换页按钮被触发
    void searchPageTriggered(int page);

public slots:
    //Client::searchFinished的槽函数
    void displaySearchResult(QByteArray result);

    void userNameChangedSlot(QString userName);
signals:
    //检索信号
    void search(QString query,int page);
private:
    //上一次的检索信息
    QString mLastQuery;
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
