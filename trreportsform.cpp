#include "trreportsform.h"
#include "ui_trreportsform.h"

TRReportsForm::TRReportsForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::TRReportsForm)
{
    ui->setupUi(this);
}

TRReportsForm::~TRReportsForm()
{
    delete ui;
}
