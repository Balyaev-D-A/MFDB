#include "materialsreportform.h"
#include "ui_materialsreportform.h"

MaterialsReportForm::MaterialsReportForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialsReportForm)
{
    ui->setupUi(this);
}

MaterialsReportForm::~MaterialsReportForm()
{
    delete ui;
}

void MaterialsReportForm::setDatabase(Database *db)
{
    this->db = db;
}

void MaterialsReportForm::updateTRMaterials()
{
    if (!ui->materialBox->count()) return;

    QString query ="SELECT  FROM trreport";
}
