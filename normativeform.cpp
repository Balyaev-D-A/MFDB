#include "normativeform.h"
#include "ui_normativeform.h"

NormativeForm::NormativeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormativeForm)
{
    ui->setupUi(this);

    ui->materialsTable->hideColumn(0);
    ui->normativeTable->hideColumn(0);
}

NormativeForm::~NormativeForm()
{
    delete ui;
}

void NormativeForm::setDatabase(Database *db)
{
    this->db = db;
}

void NormativeForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    QString query = "SELECT DISTINCT ON (sch_type) sch_type FROM schedule ORDER BY sch_type ASC";

    ui->deviceBox->clear();

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    while (db->nextRecord())
    {
        ui->deviceBox->addItem(db->fetchValue(0).toString());
    }
}

void NormativeForm::updateNormatives()
{
    while (ui->normativeTable->rowCount() > 0) ui->normativeTable->removeRow(0);
    while (ui->materialsTable->rowCount() > 0) ui->materialsTable->removeRow(0);
    ui->workEdit->clear();
    ui->actionsTextEdit->document()->clear();

    QString query = "SELECT nw_work FROM normativwork WHERE nw_dev = '%1' AND nw_work = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    if (db->nextRecord()) ui->workEdit->setText(db->fetchValue(0).toString());

    query = "SELECT nm_material, mat_name, nm_count FROM normativmat LEFT JOIN ON normativemat.nm_material = materials.mat_id "
            "WHERE nm_dev = '%1' AND nm_worktype = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    QStringList matIds;
    matIds.clear();
    while (db->nextRecord())
    {
        ui->normativeTable->insertRow(ui->normativeTable->rowCount());
        ui->normativeTable->setItem(ui->normativeTable->rowCount(), 0, new QTableWidgetItem(db->fetchValue(0).toString()));
        matIds.append(db->fetchValue(0).toString());
        ui->normativeTable->setItem(ui->normativeTable->rowCount(), 1, new QTableWidgetItem(db->fetchValue(1).toString()));
        ui->normativeTable->setItem(ui->normativeTable->rowCount(), 2, new QTableWidgetItem(db->fetchValue(2).toString()));
    }

    query = "SELECT mat_id, mat_name FROM materials";
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord())
    {
        if (!matIds.contains(db->fetchValue(0).toString())) {
            ui->materialsTable->insertRow(ui->materialsTable->rowCount());
            ui->materialsTable->setItem(ui->materialsTable->rowCount(), 0, new QTableWidgetItem(db->fetchValue(0).toString()));
            ui->materialsTable->setItem(ui->materialsTable->rowCount(), 0, new QTableWidgetItem(db->fetchValue(0).toString()));
        }
    }

    query = "SELECT na_actions FROM normativactions WHERE na_dev = '%1' AND na_worktype = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()) ui->actionsTextEdit->document()->setPlainText(db->fetchValue(0).toString());

}
