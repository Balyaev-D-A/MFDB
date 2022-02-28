#include "krreportsform.h"
#include "ui_krreportsform.h"


KRReportsForm::KRReportsForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::KRReportsForm)
{
    ui->setupUi(this);
    connect(ui->addButton, &QToolButton::clicked, this, &KRReportsForm::addButtonClicked);
}

KRReportsForm::~KRReportsForm()
{
    delete ui;
}

void KRReportsForm::setDatabase(Database *db)
{
    this->db = db;
}

void KRReportsForm::addButtonClicked()
{
    KRReportForm *krf = new KRReportForm(this);
    krf->setDatabase(db);
    connect(krf, &KRReportForm::closed, this, &KRReportsForm::reportFormClosed);
    connect(krf, &KRReportForm::saved, this, &KRReportsForm::reportSaved);
    krf->show();
}

void KRReportsForm::reportFormClosed(KRReportForm *sender)
{
    disconnect(sender, &KRReportForm::closed, this, &KRReportsForm::reportFormClosed);
    disconnect(sender, &KRReportForm::saved, this, &KRReportsForm::reportSaved);
    sender->deleteLater();
}

void KRReportsForm::reportSaved()
{

}
