#ifndef ORGANIZERWIDGET_H
#define ORGANIZERWIDGET_H

#include <QWidget>

namespace Ui {
class organizerwidget;
}

class organizerwidget : public QWidget
{
    Q_OBJECT

public:
    explicit organizerwidget(QWidget *parent = nullptr);
    ~organizerwidget();

private:
    Ui::organizerwidget *ui;
};

#endif // ORGANIZERWIDGET_H
