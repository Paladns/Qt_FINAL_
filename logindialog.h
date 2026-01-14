#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class logindialog;
}
QT_END_NAMESPACE

class logindialog : public QDialog
{
    Q_OBJECT

public:
    logindialog(QWidget *parent = nullptr);
    ~logindialog();

private:
    Ui::logindialog *ui;
};
#endif // LOGINDIALOG_H
