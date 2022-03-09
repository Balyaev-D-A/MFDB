#include "krreportpreviewform.h"
#include "ui_krreportpreviewform.h"

KRReportPreviewForm::KRReportPreviewForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::KRReportPreviewForm)
{
    ui->setupUi(this);
}

KRReportPreviewForm::~KRReportPreviewForm()
{
    delete ui;
}
void KRReportPreviewForm::setDatabase(Database *db)
{
    this->db = db;
}

QStringList KRReportPreviewForm::makeAVR()
{
    QString query = SELECT ;
}
