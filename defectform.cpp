#include "defectform.h"
#include "ui_defectform.h"

DefectForm::DefectForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DefectForm)
{
    ui->setupUi(this);
}

DefectForm::~DefectForm()
{
    delete ui;
}

void DefectForm::setDatabase(Database *db)
{
    this->db = db;
}
