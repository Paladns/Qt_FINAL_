#ifndef ADMINWIDGET_H
#define ADMINWIDGET_H

#include <QWidget>

namespace Ui {
class adminwidget;
}

class adminwidget : public QWidget
{
    Q_OBJECT

public:
    explicit adminwidget(QWidget *parent = nullptr);
    ~adminwidget();

private:
    Ui::adminwidget *ui;
};

#endif // ADMINWIDGET_H
