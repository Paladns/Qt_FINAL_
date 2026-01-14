#include "adminwidget.h"
#include "ui_adminwidget.h"

adminwidget::adminwidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminwidget)
{
    ui->setupUi(this);
}

adminwidget::~adminwidget()
{
    delete ui;
}
