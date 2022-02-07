#include "krform.h"
#include "ui_krform.h"
#include "krselectorform.h"

KRForm::KRForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KRForm)
{
    ui->setupUi(this);


}

KRForm::~KRForm()
{
    delete ui;
    ui->addedMatTable->setAcceptFrom(ui->materialTable);
    ui->materialTable->setAcceptFrom(ui->addedMatTable);
    connect(ui->selectDeviceButton, &QToolButton::clicked, this, &KRForm::selectDeviceClicked);
    connect(ui->addMaterialButton, &QToolButton::clicked, this, &KRForm::addMaterialClicked);
    connect(ui->removeMaterialButton, &QToolButton::clicked, this, &KRForm::removeMaterialClicked);
    connect(ui->cancelButton, &QToolButton::clicked, this, &KRForm::cancelClicked);
    connect(ui->okButton, &QToolButton::clicked, this, &KRForm::okClicked);
}

void KRForm::setDatabase(Database *db)
{
    this->db = db;
}

void KRForm::newKR()
{
    KRId = "0";
    selectedSched = "0";
    matsChanged = false;
    ui->deviceEdit->clear();
    ui->materialTable->setDisabled(true);
    ui->addedMatTable->setDisabled(true);
    ui->addMaterialButton->setDisabled(true);
    ui->removeMaterialButton->setDisabled(true);
    ui->okButton->setDisabled(true);
    while (ui->addedMatTable->rowCount() > 0) ui->addedMatTable->removeRow(0);
    updateMaterials();
}

void KRForm::editKR(QString KRId)
{
    this->KRId = KRId;
    matsChanged = false;
}

void KRForm::selectDeviceClicked()
{
    KRSelectorForm *ksf = new KRSelectorForm();
    ksf->setDatabase(db);
    connect(ksf, &KRSelectorForm::closed, this, &KRForm::selectorClosed);
    connect(ksf, &KRSelectorForm::selected, this, &KRForm::deviceSelected);
    ksf->show();
}

void KRForm::updateAddedMaterials()
{
    QString query = "SELECT kam_material, mat_name, kam_count FROM kradditionalmats AS kam LEFT JOIN materials AS m ON kam.kam_material = m.mat_id "
                    "WHERE kam_kr = '%1'";
    query = query.arg(KRId);

    while (ui->addedMatTable->rowCount() > 0) ui->addedMatTable->removeRow(0);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    ui->addedMatTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        ui->addedMatTable->insertRow(0);
        ui->addedMatTable->setItem(0, 0, new QTableWidgetItem(db->fetchValue(0).toString()));
        ui->addedMatTable->setItem(0, 1, new QTableWidgetItem(db->fetchValue(1).toString()));
        ui->addedMatTable->setItem(0, 2, new QTableWidgetItem(db->fetchValue(2).toString()));
    }
    ui->addedMatTable->setSortingEnabled(true);
    ui->addedMatTable->resizeColumnsToContents();
}

void KRForm::updateMaterials()
{
    QStringList matIds;
    QString query;

    while (ui->materialTable->rowCount() > 0) ui->materialTable->removeRow(0);

    query = "SELECT mat_id, mat_name FROM materials";
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    for (int i=0; i<ui->addedMatTable->rowCount(); i++)
    {
        matIds.append(ui->addedMatTable->item(i, 0)->text());
    }

    ui->materialTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        if (!matIds.contains(db->fetchValue(0).toString())) {
            ui->materialTable->insertRow(ui->materialTable->rowCount());
            ui->materialTable->setItem(ui->materialTable->rowCount()-1, 0, new QTableWidgetItem(db->fetchValue(0).toString()));
            ui->materialTable->setItem(ui->materialTable->rowCount()-1, 1, new QTableWidgetItem(db->fetchValue(1).toString()));
        }
    }
    ui->materialTable->setSortingEnabled(true);
    ui->materialTable->resizeColumnsToContents();
}

void KRForm::addMaterialClicked()
{
    ui->addedMatTable->setSortingEnabled(false);
    ui->addedMatTable->insertRow(ui->addedMatTable->rowCount());
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 0, new QTableWidgetItem(
                                   ui->materialTable->item(ui->materialTable->currentRow(), 0)->text()));
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 1, new QTableWidgetItem(
                                   ui->materialTable->item(ui->materialTable->currentRow(), 1)->text()));
    ui->addedMatTable->setSortingEnabled(true);
    ui->materialTable->removeRow(ui->materialTable->currentRow());
    matsChanged = true;
}

void KRForm::removeMaterialClicked()
{
    ui->addedMatTable->removeRow(ui->addedMatTable->currentRow());
    matsChanged = true;
    updateMaterials();
}

bool KRForm::saveKR()
{
    QString query;
    QString prepQuery;
    db->startTransaction();
    if (KRId != "0") {
        if (matsChanged) {
            query = "DELETE FROM kradditionalmats WHERE kam_kr = '%1'";
            query = query.arg(KRId);
            if (!db->execQuery(query)) {
                db->showError(this);
                db->rollbackTransaction();
                return false;
            }

            query = "INSERT INTO kradditionalmats (kam_kr, kam_material, kam_count) VALUES ('%1', '%2', '%3')";

            for (int i=0; i<ui->addedMatTable->rowCount(); i++)
            {
                prepQuery = query.arg(KRId).arg(ui->addedMatTable->item(i, 0)->text()).arg(ui->addedMatTable->item(i, 2)->text());

                if (!db->execQuery(prepQuery)) {
                    db->showError(this);
                    db->rollbackTransaction();
                    return false;
                }
            }
        }

        query = "UPDATE kaprepairs SET kr_sched = '%1' WHERE kr_id = '%2'";
        query = query.arg(selectedSched).arg(KRId);

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }
    }
    else {
        query = "INSERT INTO kaprepairs (kr_sched) VALUES ('%1')";
        query = query.arg(selectedSched);

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }

        KRId = db->lastInsertId().toString();

        query = "INSERT INTO kradditionalmats (kam_defect, kam_material, kam_count) VALUES ('%1', '%2', '%3')";

        for (int i=0; i<ui->addedMatTable->rowCount(); i++)
        {
            prepQuery = query.arg(KRId).arg(ui->addedMatTable->item(i, 0)->text()).arg(ui->addedMatTable->item(i, 2)->text());

            if (!db->execQuery(prepQuery)) {
                db->showError(this);
                db->rollbackTransaction();
                return false;
            }
        }
    }
    db->commitTransaction();
    emit krSaved();
    return true;
}



void KRForm::cancelClicked()
{
    close();
}

void KRForm::okClicked()
{
    if (saveKR()) close();
}
