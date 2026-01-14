#include "createactivitydialog.h"
#include "ui_createactivitydialog.h"

createactivitydialog::createactivitydialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::createactivitydialog)
{
    ui->setupUi(this);
}

createactivitydialog::~createactivitydialog()
{
    delete ui;
}
