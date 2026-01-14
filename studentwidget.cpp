#include "studentwidget.h"
#include "ui_studentwidget.h"

studentwidget::studentwidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::studentwidget)
{
    ui->setupUi(this);
}

studentwidget::~studentwidget()
{
    delete ui;
}
