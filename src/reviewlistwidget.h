#ifndef REVIEWLISTWIDGET_H
#define REVIEWLISTWIDGET_H
#include "inc.h"
#include "abstractlistwidget.h"

class ReviewListWidget: public AbstractListWidget
{
    Q_OBJECT
public:
    explicit ReviewListWidget(QWidget *parent = 0);
protected:
    void resizeEvent(QResizeEvent *e);
};

#endif // REVIEWLISTWIDGET_H
