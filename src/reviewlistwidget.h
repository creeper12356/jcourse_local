#ifndef REVIEWLISTWIDGET_H
#define REVIEWLISTWIDGET_H
#include "inc.h"

class ReviewListWidget: public QListWidget
{
    Q_OBJECT
public:
    explicit ReviewListWidget(QWidget *parent = 0);
};

#endif // REVIEWLISTWIDGET_H
