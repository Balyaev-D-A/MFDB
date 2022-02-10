#include <QDebug>
#include "normativeform.h"
#include "ui_normativeform.h"

NormativeForm::NormativeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormativeForm)
{
    setWindowFlag(Qt::Window, true);
    ui->setupUi(this);

    ui->materialsTable->hideColumn(0);
    ui->normativeTable->hideColumn(0);

    ui->normativeTable->setAcceptFrom(ui->materialsTable);
    ui->materialsTable->setAcceptFrom(ui->normativeTable);

    connect(ui->deviceBox, &QComboBox::currentTextChanged, this, &NormativeForm::updateNormatives);
    connect(ui->workBox, &QComboBox::currentTextChanged, this, &NormativeForm::updateNormatives);
    connect(ui->addMatButton, &QToolButton::clicked, this, &NormativeForm::addMatClicked);
    connect(ui->removeMatButton, &QToolButton::clicked, this, &NormativeForm::removeMatClicked);
    connect(ui->normativeTable, &DragDropTable::itemDroped, this, &NormativeForm::addMatClicked);
    connect(ui->materialsTable, &DragDropTable::itemDroped, this, &NormativeForm::removeMatClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &NormativeForm::close);
    connect(ui->saveButton, &QPushButton::clicked, this, &NormativeForm::saveNormatives);
    connect(ui->okButton, &QPushButton::clicked, this, &NormativeForm::okClicked);
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

    ui->deviceBox->blockSignals(true);

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

    ui->deviceBox->blockSignals(false);
    updateNormatives();
}

void NormativeForm::updateNormTable()
{
    QString query;

    while (ui->normativeTable->rowCount() > 0) ui->normativeTable->removeRow(0);
    query = "SELECT nm_material, mat_name, nm_count FROM normativmat AS nm LEFT JOIN materials AS mat ON nm.nm_material = mat.mat_id "
            "WHERE nm_dev = '%1' AND nm_worktype = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord())
    {
        ui->normativeTable->insertRow(ui->normativeTable->rowCount());
        ui->normativeTable->setItem(ui->normativeTable->rowCount()-1, 0, new QTableWidgetItem(db->fetchValue(0).toString()));
        ui->normativeTable->setItem(ui->normativeTable->rowCount()-1, 1, new QTableWidgetItem(db->fetchValue(1).toString()));
        ui->normativeTable->setItem(ui->normativeTable->rowCount()-1, 2, new QTableWidgetItem(db->fetchValue(2).toString()));
    }
}

void NormativeForm::updateMatTable()
{
    QString query;
    QStringList matIds;
    matIds.clear();

    while (ui->materialsTable->rowCount() > 0) ui->materialsTable->removeRow(0);

    query = "SELECT mat_id, mat_name FROM materials";
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    for (int i=0; i<ui->normativeTable->rowCount(); i++)
    {
        matIds.append(ui->normativeTable->item(i, 0)->text());
    }

    ui->materialsTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        if (!matIds.contains(db->fetchValue(0).toString())) {
            ui->materialsTable->insertRow(ui->materialsTable->rowCount());
            ui->materialsTable->setItem(ui->materialsTable->rowCount()-1, 0, new QTableWidgetItem(db->fetchValue(0).toString()));
            ui->materialsTable->setItem(ui->materialsTable->rowCount()-1, 1, new QTableWidgetItem(db->fetchValue(1).toString()));
        }
    }
    ui->materialsTable->setSortingEnabled(true);
    ui->materialsTable->resizeColumnsToContents();
}

void NormativeForm::updateNormatives()
{
    ui->workEdit->clear();
    ui->oesnEdit->clear();
    ui->actionsTextEdit->document()->clear();

    QString query = "SELECT nw_oesn, nw_work FROM normativwork WHERE nw_dev = '%1' AND nw_worktype = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    if (db->nextRecord()){
        ui->oesnEdit->setText(db->fetchValue(0).toString());
        ui->workEdit->setText(db->fetchValue(1).toString());
    }

    updateNormTable();
    updateMatTable();

    query = "SELECT na_actions FROM normativactions WHERE na_dev = '%1' AND na_worktype = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()) ui->actionsTextEdit->document()->setPlainText(db->fetchValue(0).toString());

}

bool NormativeForm::saveNormatives()
{
    QString query;
    QString q;

    db->startTransaction();

    query = "DELETE FROM normativwork WHERE nw_dev = '%1' AND nw_worktype = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());
    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return false;
    }

    query = "DELETE FROM normativmat WHERE nm_dev = '%1' AND nm_worktype = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());
    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return false;
    }

    query = "DELETE FROM normativactions WHERE na_dev = '%1' AND na_worktype = '%2'";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());
    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return false;
    }

    query = "INSERT INTO normativwork (nw_dev, nw_worktype, nw_oesn, nw_work) VALUES ('%1' , '%2', '%3', '%4')";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText()).arg(ui->oesnEdit->text());
    query = query.arg(ui->workEdit->text().replace(',', '.'));
    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return false;
    }

    query = "INSERT INTO normativactions (na_dev, na_worktype, na_actions) VALUES ('%1' , '%2', '%3')";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText()).arg(ui->actionsTextEdit->document()->toPlainText());
    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return false;
    }

    query = "INSERT INTO normativmat (nm_dev, nm_worktype, nm_material, nm_count) VALUES ('%1', '%2', '%3', '%4')";
    query = query.arg(ui->deviceBox->currentText()).arg(ui->workBox->currentText());
    for (int i=0; i<ui->normativeTable->rowCount(); i++)
    {
        q = query.arg(ui->normativeTable->item(i, 0)->text()).arg(ui->normativeTable->item(i,2)->text().replace(',','.'));
        if (!db->execQuery(q)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }
    }

    db->commitTransaction();
    return true;
}

void NormativeForm::addMatClicked()
{
    int curRow;
    QString matId, matName;
    ui->normativeTable->insertRow(ui->normativeTable->rowCount());
    curRow = ui->normativeTable->rowCount()-1;
    matId = ui->materialsTable->item(ui->materialsTable->currentRow(), 0)->text();
    matName = ui->materialsTable->item(ui->materialsTable->currentRow(), 1)->text();
    ui->normativeTable->setItem(curRow, 0, new QTableWidgetItem(matId));
    ui->normativeTable->setItem(curRow, 1, new QTableWidgetItem(matName));
    ui->materialsTable->removeRow(ui->materialsTable->currentRow());
    ui->normativeTable->resizeColumnsToContents();
}

void NormativeForm::removeMatClicked()
{
    ui->normativeTable->removeRow(ui->normativeTable->currentRow());
    updateMatTable();
}

void NormativeForm::okClicked()
{
    if (saveNormatives()) close();

}
