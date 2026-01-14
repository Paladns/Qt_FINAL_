#ifndef STUDENTWIDGET_H
#define STUDENTWIDGET_H

#include <QWidget>

namespace Ui {
class studentwidget;
}

class studentwidget : public QWidget
{
    Q_OBJECT

public:
    explicit studentwidget(QWidget *parent = nullptr);
    ~studentwidget();

private:
    Ui::studentwidget *ui;
};

#endif // STUDENTWIDGET_H
