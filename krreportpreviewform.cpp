#include "krreportpreviewform.h"
#include "ui_krreportpreviewform.h"

KRReportPreviewForm::KRReportPreviewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KRReportPreviewForm)
{
    ui->setupUi(this);
}

KRReportPreviewForm::~KRReportPreviewForm()
{
    delete ui;
}
