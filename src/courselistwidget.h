#ifndef COURSELISTWIDGET_H
#define COURSELISTWIDGET_H
#include "inc.h"
#include "abstractlistwidget.h"

/*!
 * \brief 课程列表控件
 *
 * Inherits: AbstractListWidget
 */
class CourseListWidget: public AbstractListWidget
{
    Q_OBJECT
public:
    explicit CourseListWidget(QWidget* parent = 0);
    ~CourseListWidget();
    void displayResult(const QJsonObject& resultJsonObject) override;
protected slots:
    void itemClickedSlot(QListWidgetItem* item) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
signals:
//    课程被选中的信号，发给MainWindow
    void courseSelected(int courseid);
    //缓存课程评价的信号，发给MainWindow
    void cacheCourseReview(int courseid);

private:
    QAction* mCacheAction;
};

#endif // COURSELISTWIDGET_H
