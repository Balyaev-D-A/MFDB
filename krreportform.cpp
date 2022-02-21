#include "krreportform.h"
#include "ui_krreportform.h"

KRReportForm::KRReportForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KRReportForm)
{
    ui->setupUi(this);
}

KRReportForm::~KRReportForm()
{
    delete ui;
}
