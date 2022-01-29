#include "defectform.h"
#include "ui_defectform.h"

DefectForm::DefectForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DefectForm)
{
    ui->setupUi(this);
    connect(ui->deviceButton, &QToolButton::clicked, this, &DefectForm::deviceButtonClicked);
    connect(ui->addDefectButton, &QToolButton::clicked, this, &DefectForm::addDefectClicked);
    connect(ui->saveDefectButton, &QToolButton::clicked, this, &DefectForm::saveDefectClicked);
    connect(ui->deleteDefectButton, &QToolButton::clicked, this, &DefectForm::deleteDefectClicked);
    connect(ui->addRepairButton, &QToolButton::clicked, this, &DefectForm::addRepairClicked);
    connect(ui->saveRepairButton, &QToolButton::clicked, this, &DefectForm::saveRepairClicked);
    connect(ui->deleteRepairButton, &QToolButton::clicked, this, &DefectForm::deleteRepairClicked);
    connect(ui->nextDefectButton, &QToolButton::clicked, this, &DefectForm::nextDefectClicked);
    connect(ui->prevDefectButton, &QToolButton::clicked, this, &DefectForm::prevDefectClicked);
    connect(ui->nextRepairButton, &QToolButton::clicked, this, &DefectForm::nextRepairClicked);
    connect(ui->prevRepairButton, &QToolButton::clicked, this, &DefectForm::prevRepairClicked);
    connect(ui->addMaterialButton, &QToolButton::clicked, this, &DefectForm::addMaterialClicked);
    connect(ui->removeMaterialButton, &QToolButton::clicked, this, &DefectForm::removeMaterialClicked);
    connect(ui->addedMatTable, &DragDropTable::itemDroped, this, &DefectForm::addMaterialClicked);
    connect(ui->materialTable, &DragDropTable::itemDroped, this, &DefectForm::removeMaterialClicked);
    connect(ui->okButton, &QPushButton::clicked, this, &DefectForm::okClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &DefectForm::cancelClicked);

    ui->buttonGroup->setId(ui->externRevievRB, EXTERNREVIEW);
    ui->buttonGroup->setId(ui->externConnRB, EXTERNCONN);
    ui->buttonGroup->setId(ui->internConnRB, INTERNCONN);
    ui->buttonGroup->setId(ui->electricParRB, ELECTRICPAR);

    ui->addedMatTable->hideColumn(0);
    ui->materialTable->hideColumn(0);
    ui->addedMatTable->sortByColumn(1, Qt::AscendingOrder);
    ui->materialTable->sortByColumn(1, Qt::AscendingOrder);
    ui->addedMatTable->setAcceptFrom(ui->materialTable);
    ui->materialTable->setAcceptFrom(ui->addedMatTable);

}

DefectForm::~DefectForm()
{
    delete ui;
}

void DefectForm::setDatabase(Database *db)
{
    this->db = db;
}

void DefectForm::newDefect()
{
    defId = "0";
    device.kks = "";
    device.name = "";
    device.type = "";
    ui->deviceEdit->setText("");
    ui->journalDefectEdit->setDisabled(true);
    ui->materialTable->setDisabled(true);
    ui->addedMatTable->setDisabled(true);
    ui->addMaterialButton->setDisabled(true);
    ui->removeMaterialButton->setDisabled(true);
    updateDefects();
    updateRepairs();
    updateAddedMaterials();
    updateMaterials();
}

void DefectForm::editDefect(QString defId)
{
    QString query;
    int stage;

    this->defId = defId;

    query = "SELECT def_devtype, def_kks, def_journaldesc, def_realdesc, def_stage, def_repairdesc WHERE def_id = '%1'";
    query = query.arg(defId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()) {
        device.type = db->fetchValue(0).toString();
        device.kks = db->fetchValue(1).toString();
        ui->journalDefectEdit->setText(db->fetchValue(2).toString());
        ui->defectEdit->setText(db->fetchValue(3).toString());
        stage = db->fetchValue(4).toInt();
        ui->repairEdit->setText(db->fetchValue(5).toString());

        switch (stage) {
        case EXTERNREVIEW:
            ui->externRevievRB->setChecked(true);
            break;
        case EXTERNCONN:
            ui->externConnRB->setChecked(true);
            break;
        case INTERNCONN:
            ui->internConnRB->setChecked(true);
            break;
        case ELECTRICPAR:
            ui->electricParRB->setChecked(true);
            break;
        }
    }

    query = "SELECT sch_name from schedule WHERE sch_type = '%1' AND sch_kks = '%2' LIMIT 1";
    query = query.arg(device.type).arg(device.kks);

    if (db->execQuery(query))
        if (db->nextRecord())
            device.name = db->fetchValue(0).toString();
    ui->deviceEdit->setText(device.name + " " + device.type + " " + device.kks);
    updateAddedMaterials();
    updateMaterials();
    updateDefects();
    updateRepairs();
}

void DefectForm::deviceButtonClicked()
{
    dsf = new DeviceSelectorForm();
    dsf->setDatabase(db);
    connect(dsf, &DeviceSelectorForm::closed, this, &DefectForm::selectorClosed);
    connect(dsf, &DeviceSelectorForm::deviceChoosed, this, &DefectForm::deviceChoosed);
    dsf->show();
}

void DefectForm::selectorClosed(DeviceSelectorForm *sender)
{
    sender->deleteLater();
}

void DefectForm::deviceChoosed(SelectedDevice device)
{
    this->device = device;
    ui->deviceEdit->setText(device.name + " " + device.type + " " + device.kks);
    ui->journalDefectEdit->setDisabled(false);
    ui->materialTable->setDisabled(false);
    ui->addedMatTable->setDisabled(false);
    ui->addMaterialButton->setDisabled(false);
    ui->removeMaterialButton->setDisabled(false);
    updateDefects();
    updateRepairs();
}

void DefectForm::updateDefects()
{
    Defect defect;
    QString query = "SELECT drd_id, drd_stage, drd_desc FROM defrealdescs WHERE drd_dev = '%1'";
    query = query.arg(device.type);
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    defectList.clear();

    while (db->nextRecord())
    {
        defect.id = db->fetchValue(0).toInt();
        defect.stage = db->fetchValue(1).toInt();
        defect.description = db->fetchValue(2).toString();
        defectList.append(defect);
    }
    currentDefect = 0;
    updateDefectText();
}

void DefectForm::updateDefectText()
{
    if (defectList.isEmpty()) {
        ui->defectLabel->setText("Дефект 0 из 0");
        ui->defectEdit->setText("");
        ui->defectEdit->setDisabled(true);
        if (device.name == "")
            ui->addDefectButton->setDisabled(true);
        else
            ui->addDefectButton->setDisabled(false);
        ui->externRevievRB->setDisabled(true);
        ui->externConnRB->setDisabled(true);
        ui->internConnRB->setDisabled(true);
        ui->electricParRB->setDisabled(true);
        ui->saveDefectButton->setDisabled(true);
        ui->deleteDefectButton->setDisabled(true);
        ui->nextDefectButton->setDisabled(true);
        ui->prevDefectButton->setDisabled(true);
        return;
    }

    ui->defectEdit->setDisabled(false);
    ui->externRevievRB->setDisabled(false);
    ui->externConnRB->setDisabled(false);
    ui->internConnRB->setDisabled(false);
    ui->electricParRB->setDisabled(false);
    ui->addDefectButton->setDisabled(false);
    ui->saveDefectButton->setDisabled(false);
    ui->deleteDefectButton->setDisabled(false);
    if (currentDefect + 1 >= defectList.size())
        ui->nextDefectButton->setDisabled(true);
    else
        ui->nextDefectButton->setDisabled(false);
    if (currentDefect <= 0)
        ui->prevDefectButton->setDisabled(true);
    else
        ui->prevDefectButton->setDisabled(false);

    ui->defectLabel->setText(QString("Дефект %1 из %2").arg(currentDefect + 1).arg(defectList.size()));
    ui->defectEdit->setText(defectList[currentDefect].description);
    switch (defectList[currentDefect].stage) {
    case EXTERNREVIEW:
        ui->externRevievRB->setChecked(true);
        break;
    case EXTERNCONN:
        ui->externConnRB->setChecked(true);
        break;
    case INTERNCONN:
        ui->internConnRB->setChecked(true);
        break;
    case ELECTRICPAR:
        ui->electricParRB->setChecked(true);
        break;
    }
}

void DefectForm::updateRepairs()
{
    Repair repair;
    QString query = "SELECT drp_id, drp_repairdesc FROM defrepairdescs WHERE drp_realdefdesc = '%1'";

    if (defectList.isEmpty()) {
        updateRepairText();
        return;
    }

    query = query.arg(defectList[currentDefect].id);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    repairList.clear();
    while (db->nextRecord())
    {
        repair.id = db->fetchValue(0).toInt();
        repair.description = db->fetchValue(1).toString();
        repairList.append(repair);
    }
    currentRepair = 0;
    updateRepairText();
}

void DefectForm::updateRepairText()
{
    if (defectList.isEmpty()) {
        ui->repairLabel->setText("Устранение 0 из 0");
        ui->repairEdit->setText("");
        ui->repairEdit->setDisabled(true);
        ui->addRepairButton->setDisabled(true);
        ui->deleteRepairButton->setDisabled(true);
        ui->saveRepairButton->setDisabled(true);
        ui->nextRepairButton->setDisabled(true);
        ui->prevRepairButton->setDisabled(true);
        return;
    }

    if (repairList.isEmpty()) {
        ui->repairLabel->setText("Устранение 0 из 0");
        ui->repairEdit->setText("");
        ui->repairEdit->setDisabled(true);
        ui->addRepairButton->setDisabled(false);
        ui->deleteRepairButton->setDisabled(true);
        ui->saveRepairButton->setDisabled(true);
        ui->nextRepairButton->setDisabled(true);
        ui->prevRepairButton->setDisabled(true);
        return;
    }

    ui->addRepairButton->setDisabled(false);
    ui->deleteRepairButton->setDisabled(false);
    ui->saveRepairButton->setDisabled(false);
    if (currentRepair + 1 >= repairList.size())
        ui->nextRepairButton->setDisabled(true);
    else
        ui->nextRepairButton->setDisabled(false);
    if (currentRepair <= 0)
        ui->prevRepairButton->setDisabled(true);
    else
        ui->prevRepairButton->setDisabled(false);
    ui->repairLabel->setText(QString("Устранение %1 из %2").arg(currentRepair + 1).arg(repairList.size()));
    ui->repairEdit->setDisabled(false);
    ui->repairEdit->setText(repairList[currentRepair].description);
}

void DefectForm::addDefectClicked()
{
    Defect defect;
    QString query = "INSERT INTO defrealdescs (drd_dev) VALUES ('%1')";
    query = query.arg(device.type);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    defect.id = db->lastInsertId().toInt();
    defectList.append(defect);
    currentDefect = defectList.size() - 1;
    updateDefectText();
    updateRepairs();
}

void DefectForm::saveDefectClicked()
{
    QString query = "UPDATE defrealdescs SET drd_stage = '%1', drd_desc = '%2' WHERE drd_id = '%3'";
    query = query.arg(ui->buttonGroup->checkedId());
    query = query.arg(ui->defectEdit->text());
    query = query.arg(defectList[currentDefect].id);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    defectList[currentDefect].stage = ui->buttonGroup->checkedId();
    defectList[currentDefect].description = ui->defectEdit->text();
}

void DefectForm::deleteDefectClicked()
{
    QString query = "DELETE FROM defrealdescs WHERE drd_id = '%1'";
    query = query.arg(defectList[currentDefect].id);
    db->startTransaction();
    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return;
    }

    query = "DELETE FROM defrepairdescs WHERE drp_realdefdesc = '%1'";
    query = query.arg(defectList[currentDefect].id);

    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return;
    }
    db->commitTransaction();
    defectList.removeAt(currentDefect);
    if (currentDefect + 1 > defectList.size()) currentDefect = defectList.size() - 1;
    updateDefectText();
    updateRepairs();
}

void DefectForm::addRepairClicked()
{
    Repair repair;
    QString query = "INSERT INTO defrepairdescs (drp_realdefdesc) VALUES ('%1')";
    query = query.arg(defectList[currentDefect].id);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    repair.id = db->lastInsertId().toInt();
    repair.description = "";
    repairList.append(repair);
    currentRepair = repairList.size() - 1;
    updateRepairText();
}

void DefectForm::saveRepairClicked()
{
    QString query = "UPDATE defrepairdescs SET drp_repairdesc = '%1' WHERE drp_id = '%2'";
    query = query.arg(ui->repairEdit->text());
    query = query.arg(repairList[currentRepair].id);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    repairList[currentRepair].description = ui->repairEdit->text();
}

void DefectForm::deleteRepairClicked()
{
    QString query = "DELETE FROM defrepairdescs WHERE drp_id = '%1'";
    query = query.arg(repairList[currentRepair].id);

    if(!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    repairList.removeAt(currentRepair);
    if (currentRepair + 1 > repairList.size()) currentRepair = repairList.size() - 1;
    updateRepairText();
}

void DefectForm::updateAddedMaterials()
{
    QString query = "SELECT dam_material, mat_name, dam_count FROM defadditionalmats AS dam LEFT JOIN materials AS m ON dam.dam_material = m.mat_id "
                    "WHERE dam_defect = '%1'";
    query = query.arg(defId);

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

void DefectForm::updateMaterials()
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

void DefectForm::addMaterialClicked()
{
    ui->addedMatTable->setSortingEnabled(false);
    ui->addedMatTable->insertRow(ui->addedMatTable->rowCount());
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 0, new QTableWidgetItem(
                                   ui->materialTable->item(ui->materialTable->currentRow(), 0)->text()));
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 1, new QTableWidgetItem(
                                   ui->materialTable->item(ui->materialTable->currentRow(), 1)->text()));
    ui->addedMatTable->setSortingEnabled(true);
    ui->materialTable->removeRow(ui->materialTable->currentRow());
}

void DefectForm::removeMaterialClicked()
{
    ui->addedMatTable->removeRow(ui->addedMatTable->currentRow());
    updateMaterials();
}

bool DefectForm::saveDefect()
{
    QString query;
    QString prepQuery;
    db->startTransaction();
    if (defId != "0") {
        if (matsChanged) {
            query = "DELETE FROM defadditionalmats WHERE dam_defect = '%1'";
            query = query.arg(defId);
            if (!db->execQuery(query)) {
                db->showError(this);
                db->rollbackTransaction();
                return false;
            }

            query = "INSERT INTO defadditionalmats (dam_defect, dam_material, dam_count) VALUES ('%1', '%2', '%3')";

            for (int i=0; i<ui->addedMatTable->rowCount(); i++)
            {
                prepQuery = query.arg(defId).arg(ui->addedMatTable->item(i, 0)->text()).arg(ui->addedMatTable->item(i, 2)->text());

                if (!db->execQuery(prepQuery)) {
                    db->showError(this);
                    db->rollbackTransaction();
                    return false;
                }
            }
        }
        query = "UPDATE defects SET def_devtype = '%1', def_kks = '%2', def_journaldesc = '%3', def_realdesc = '%4', "
                "def_stage = '%5', def_repairdesc = '%6'";
        query = query.arg(device.type).arg(device.kks).arg(ui->journalDefectEdit->text());
        query = query.arg(ui->defectEdit->text()).arg(ui->buttonGroup->checkedId()).arg(ui->repairEdit->text());

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }
    }
    else {
        query = "INSERT INTO defects (def_devtype, def_kks, def_journaldesc, def_realdesc, def_stage, def_repairdesc) "
                "VALUES ('%1', '%2', '%3', '%4', '%5', '%6')";
        query = query.arg(device.type).arg(device.kks).arg(ui->journalDefectEdit->text()).arg(ui->defectEdit->text());
        query = query.arg(ui->buttonGroup->checkedId()).arg(ui->repairEdit->text());

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }

        query = "INSERT INTO defadditionalmats (dam_defect, dam_material, dam_count) VALUES ('%1', '%2', '%3')";

        for (int i=0; i<ui->addedMatTable->rowCount(); i++)
        {
            prepQuery = query.arg(defId).arg(ui->addedMatTable->item(i, 0)->text()).arg(ui->addedMatTable->item(i, 2)->text());

            if (!db->execQuery(prepQuery)) {
                db->showError(this);
                db->rollbackTransaction();
                return false;
            }
        }
    }
    db->commitTransaction();
    return true;
}

void DefectForm::okClicked()
{
    if (saveDefect()) close();
}

void DefectForm::cancelClicked()
{
    close();
}
