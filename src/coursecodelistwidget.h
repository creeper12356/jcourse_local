#ifndef COURSECODELISTWIDGET_H
#define COURSECODELISTWIDGET_H
#include "inc.h"
#include "abstractlistwidget.h"

class CourseCodeListWidget : public AbstractListWidget
{
    Q_OBJECT
public:
    explicit CourseCodeListWidget(QWidget* parent = 0);
    ~CourseCodeListWidget();
    void displayResult(const QJsonObject& resultJsonObject) override;
protected:
    void itemClickedSlot(QListWidgetItem *item) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
signals:
    //缓存所有课号为courseCode的课程评价信号，发送给MainWindow
    void cacheCourseCodeReview(QString courseCode);
private:
    QAction* mCacheAction;
};

#endif // COURSECODELISTWIDGET_H
