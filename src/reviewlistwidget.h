#ifndef REVIEWLISTWIDGET_H
#define REVIEWLISTWIDGET_H
#include "inc.h"
#include "abstractlistwidget.h"

/*!
 * \brief 课程评价列表控件
 *
 * Inherits: AbstractListWidget
 */
class ReviewListWidget: public AbstractListWidget
{
    Q_OBJECT
public:
    explicit ReviewListWidget(QWidget *parent = 0);
    void displayResult(const QJsonObject& resultJsonObject) override;
protected:
    void resizeEvent(QResizeEvent *e);
private slots:
    void itemClickedSlot(QListWidgetItem* item) override;

};

#endif // REVIEWLISTWIDGET_H
