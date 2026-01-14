#ifndef CREATEACTIVITYDIALOG_H
#define CREATEACTIVITYDIALOG_H

#include <QDialog>

namespace Ui {
class createactivitydialog;
}

class createactivitydialog : public QDialog
{
    Q_OBJECT

public:
    explicit createactivitydialog(QWidget *parent = nullptr);
    ~createactivitydialog();

private:
    Ui::createactivitydialog *ui;
};

#endif // CREATEACTIVITYDIALOG_H
