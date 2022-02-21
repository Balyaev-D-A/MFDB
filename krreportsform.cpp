#include "krreportsform.h"
#include "ui_krreportsform.h"

KRReportsForm::KRReportsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KRReportsForm)
{
    ui->setupUi(this);
}

KRReportsForm::~KRReportsForm()
{
    delete ui;
}
