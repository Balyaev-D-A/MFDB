#include "krreportsform.h"
#include "ui_krreportsform.h"
#include "krreportform.h"

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
    krf->show();
}
