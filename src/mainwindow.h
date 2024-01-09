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

private:
    Ui::MainWindow *ui;
private slots:
    void searchTriggered();
public slots:
    void cacheChangedSlot(QString cache);
signals:
    void search(QString query);
};

#endif // MAINWINDOW_H
