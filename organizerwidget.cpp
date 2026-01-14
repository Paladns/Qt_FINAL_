#include "organizerwidget.h"
#include "ui_organizerwidget.h"

organizerwidget::organizerwidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::organizerwidget)
{
    ui->setupUi(this);
}

organizerwidget::~organizerwidget()
{
    delete ui;
}
