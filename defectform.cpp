#include "defectform.h"
#include "ui_defectform.h"
#include "fieldeditor.h"

DefectForm::DefectForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
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
//    connect(ui->journalDefectEdit, &QLineEdit::textChanged, this, &DefectForm::updateActionsDesc);
    connect(ui->defectEdit, &QLineEdit::textChanged, this, &DefectForm::defectTextChanged);
    connect(ui->repairEdit, &QLineEdit::textChanged, this, &DefectForm::repairTextChanged);
    connect(ui->stageBox, &QComboBox::currentTextChanged, this, &DefectForm::defectTextChanged);
    connect(ui->oesnButton, &QToolButton::clicked, this, &DefectForm::oesnClicked);
    connect(ui->addedMatTable, &DragDropTable::cellDoubleClicked, this, &DefectForm::cellDoubleClicked);
    connect(ui->fillButton, &QToolButton::clicked, this, &DefectForm::fillButtonClicked);
    connect(ui->addedMatTable, &DragDropTable::rowMoved, this, &DefectForm::addedMatsRowMoved);

    ui->addedMatTable->hideColumn(0);
    ui->materialTable->hideColumn(0);
    ui->materialTable->sortByColumn(1, Qt::AscendingOrder);
    ui->addedMatTable->setAcceptFrom(ui->materialTable);
    ui->materialTable->setAcceptFrom(ui->addedMatTable);
    ui->addedMatTable->setMovableRows(true);

}

DefectForm::~DefectForm()
{
    delete ui;
}

void DefectForm::setDatabase(Database *db)
{
    this->db = db;
}

void DefectForm::newDefect(int quarter)
{
    setWindowTitle("Новый дефект");
    defId = "0";
    device.kks = "";
    device.name = "";
    device.type = "";
    matsChanged = false;
    ui->quarterBox->setCurrentIndex(quarter);
    ui->deviceEdit->setText("");
    ui->journalDefectEdit->setText("");
    ui->stageBox->clear();
    ui->actionsTextEdit->clear();
    ui->oesnButton->setDisabled(true);
    ui->journalDefectEdit->setDisabled(true);
    ui->materialTable->setDisabled(true);
    ui->addedMatTable->setDisabled(true);
    ui->addMaterialButton->setDisabled(true);
    ui->removeMaterialButton->setDisabled(true);
    ui->fillButton->setDisabled(true);
    ui->actionsTextEdit->setDisabled(true);
    while (ui->addedMatTable->rowCount()) ui->addedMatTable->removeRow(0);
    ui->addedMatTable->clearPersistentRows();
    updateDefects();
    updateRepairs();
    updateMaterials();
    ui->actionsTextEdit->document()->setPlainText("");
}

void DefectForm::editDefect(QString defId)
{
    QString query, defect, repair, actiondesc;
    int stage;
    bool found = false;

    setWindowTitle("Редактировать дефект");

    matsChanged = false;

    this->defId = defId;

    query = "SELECT def_devtype, def_kks, def_journaldesc, def_realdesc, def_stage, def_repairdesc, def_quarter, def_unit, def_actionsdesc FROM defects WHERE def_id = '%1'";
    query = query.arg(defId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()) {
        device.type = db->fetchValue(0).toString();
        device.kks = db->fetchValue(1).toString();
        device.unitId = db->fetchValue(7).toString();
        ui->journalDefectEdit->setText(db->fetchValue(2).toString());
        defect = db->fetchValue(3).toString();
        stage = db->fetchValue(4).toInt();
        repair = db->fetchValue(5).toString();
        actiondesc = db->fetchValue(8).toString();
        ui->quarterBox->setCurrentIndex(db->fetchValue(6).toInt() - 1);
        updateStages();
        ui->stageBox->setCurrentIndex(stage);
    }

    query = "SELECT sch_name from schedule WHERE sch_type = '%1' AND sch_kks = '%2' LIMIT 1";
    query = query.arg(device.type).arg(device.kks);

    if (db->execQuery(query))
        if (db->nextRecord())
            device.name = db->fetchValue(0).toString();
    ui->deviceEdit->setText(device.name + " " + device.type + " " + device.kks);
    ui->oesnButton->setDisabled(false);
    ui->journalDefectEdit->setDisabled(false);
    ui->addedMatTable->setDisabled(false);
    ui->materialTable->setDisabled(false);
    ui->addMaterialButton->setDisabled(false);
    ui->removeMaterialButton->setDisabled(false);
    ui->fillButton->setDisabled(false);
    ui->actionsTextEdit->setDisabled(false);
    ui->addedMatTable->clearPersistentRows();
    updateAddedMaterials();
    updateMaterials();
    updateDefects();
    for (int i=0; i<defectList.count(); i++)
    {
        if (defect.simplified() == defectList[i].description.simplified()){
            currentDefect = i;
            updateDefectText();
            found = true;
            break;
        }
    }
    if (!found) {
        addDefectClicked();
        ui->defectEdit->setText(defect.simplified());
        saveDefectClicked();
    }
    updateRepairs();
    found = false;
    for (int i=0; i<repairList.count(); i++)
    {
        if (repair.simplified() == repairList[i].description.simplified()) {
            currentRepair = i;
            updateRepairText();
            found = true;
            break;
        }
    }
    if (!found) {
        addRepairClicked();
        ui->repairEdit->setText(repair);
        saveRepairClicked();
    }
    updateActions();
    ui->actionsTextEdit->document()->setPlainText(actiondesc);
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
    ui->oesnButton->setDisabled(false);
    ui->fillButton->setDisabled(false);
    ui->actionsTextEdit->setDisabled(false);
    updateStages();
    updateDefects();
    updateRepairs();
    updateActions();
    updateActionsDesc();
    updateAddedMaterials();
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
        ui->defectLabel->setText("Дефект по факту 0 из 0");
        ui->defectEdit->setText("");
        ui->defectEdit->setDisabled(true);
        if (device.name == "")
            ui->addDefectButton->setDisabled(true);
        else
            ui->addDefectButton->setDisabled(false);
        ui->stageBox->setDisabled(true);
        ui->saveDefectButton->setDisabled(true);
        ui->deleteDefectButton->setDisabled(true);
        ui->nextDefectButton->setDisabled(true);
        ui->prevDefectButton->setDisabled(true);
        return;
    }

    ui->defectEdit->setDisabled(false);
    ui->stageBox->setDisabled(false);
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

    ui->defectLabel->setText(QString("Дефект по факту %1 из %2").arg(currentDefect + 1).arg(defectList.size()));
    ui->defectEdit->setText(defectList[currentDefect].description);
    ui->stageBox->setCurrentIndex(defectList[currentDefect].stage);

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

void DefectForm::updateStages()
{
    QString actions;
    QStringList actList;
    QString query = "SELECT na_actions FROM normativactions WHERE na_dev = '%1' AND na_worktype = '%2'";

    query = query.arg(device.type).arg("ТР");

    if (!db->execQuery(query)){
        db->showError(this);
        return;
    }

    ui->stageBox->clear();
    if (!db->nextRecord()) return;

    actions = db->fetchValue(0).toString();
    actList = actions.split("\n");
    for (int i=0; i<actList.size(); i++)
    {
        if (actList[i].startsWith('@'))
                ui->stageBox->addItem(actList[i].remove(0, 1));
    }
}

void DefectForm::updateActions()
{
    QString query = "SELECT na_actions FROM normativactions WHERE na_dev = '%1' AND na_worktype = '%2'";

    query = query.arg(device.type).arg("ТР");

    if (!db->execQuery(query)){
        db->showError(this);
        return;
    }

    if (db->nextRecord())
        actions = db->fetchValue(0).toString();
    else
        actions = "";


}

void DefectForm::updateActionsDesc()
{
    QString act;
    QStringList actList;

    actList = actions.split("\n");
    act = "";
    for (int i=0; i<actList.size(); i++)
    {
        if (actList[i].startsWith('@')){
            if (actList[i].remove(0, 1) == ui->stageBox->currentText()) {
                actList[i] = actList[i] + " " + ui->defectEdit->text().simplified() + "\nДефектация: " + ui->defectEdit->text().simplified() +
                        "\n" + ui->repairEdit->text().simplified();
            }
            else {
                actList[i] = actList[i] + " Замечаний нет.";
            }
        }
    }
    act = actList.join("\n");
    actList = act.split("\n");
    for (int i=0; i<actList.size(); i++)
    {
        actList[i].prepend(QString("%1. ").arg(i+1));
    }

    act = actList.join("\n");

    ui->actionsTextEdit->document()->setPlainText(act);
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
    ui->defectEdit->setText(ui->defectEdit->text().simplified());
    QString query = "UPDATE defrealdescs SET drd_stage = '%1', drd_desc = '%2' WHERE drd_id = '%3'";
    query = query.arg(ui->stageBox->currentIndex());
    query = query.arg(ui->defectEdit->text());
    query = query.arg(defectList[currentDefect].id);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    defectList[currentDefect].stage = ui->stageBox->currentIndex();
    defectList[currentDefect].description = ui->defectEdit->text();
    ui->saveDefectButton->setEnabled(false);
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
    ui->repairEdit->setText(ui->repairEdit->text().simplified());
    QString query = "UPDATE defrepairdescs SET drp_repairdesc = '%1' WHERE drp_id = '%2'";
    query = query.arg(ui->repairEdit->text());
    query = query.arg(repairList[currentRepair].id);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    repairList[currentRepair].description = ui->repairEdit->text();
    ui->saveRepairButton->setEnabled(false);
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
    int curRow;
    QString query;

    while (ui->addedMatTable->rowCount() > 0) ui->addedMatTable->removeRow(0);

    query = "SELECT dam_material, mat_name, dam_oesn, dam_count FROM defadditionalmats AS dam LEFT JOIN materials AS m ON dam.dam_material = m.mat_id "
                    "WHERE dam_defect = '%1' ORDER BY dam_order";
    query = query.arg(defId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord())
    {
        curRow = ui->addedMatTable->rowCount();
        ui->addedMatTable->insertRow(curRow);
        ui->addedMatTable->setItem(curRow, 0, new QTableWidgetItem(db->fetchValue(0).toString()));
        ui->addedMatTable->setItem(curRow, 1, new QTableWidgetItem(db->fetchValue(1).toString()));
        if (db->fetchValue(2).toFloat() == 0) ui->addedMatTable->setItem(curRow, 2, new QTableWidgetItem("-"));
        else ui->addedMatTable->setItem(curRow, 2, new QTableWidgetItem(db->fetchValue(2).toString()));
        ui->addedMatTable->setItem(curRow, 3, new QTableWidgetItem(db->fetchValue(3).toString()));
    }
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
    if (ui->materialTable->currentRow() < 0) return;
    ui->addedMatTable->insertRow(ui->addedMatTable->rowCount());
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 0, new QTableWidgetItem(
                                   ui->materialTable->item(ui->materialTable->currentRow(), 0)->text()));
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 1, new QTableWidgetItem(
                                   ui->materialTable->item(ui->materialTable->currentRow(), 1)->text()));
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 2, new QTableWidgetItem("-"));
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 3, new QTableWidgetItem(""));
    ui->materialTable->removeRow(ui->materialTable->currentRow());
    ui->addedMatTable->resizeColumnsToContents();
    ui->materialTable->resizeColumnsToContents();
    matsChanged = true;
}

void DefectForm::removeMaterialClicked()
{
    if (ui->addedMatTable->currentRow() < 0) return;
    if (ui->addedMatTable->isPersistentRow(ui->addedMatTable->currentRow())) return;
    ui->addedMatTable->removeRow(ui->addedMatTable->currentRow());
    matsChanged = true;
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

            query = "INSERT INTO defadditionalmats (dam_defect, dam_material, dam_oesn, dam_count, dam_order) VALUES ('%1', '%2', '%3', '%4', '%5')";

            for (int i=0; i<ui->addedMatTable->rowCount(); i++)
            {
                prepQuery = query.arg(defId).arg(ui->addedMatTable->item(i, 0)->text());
                prepQuery = prepQuery.arg(ui->addedMatTable->item(i, 2)->text().replace(",", ".").replace("-", "0"));
                prepQuery = prepQuery.arg(ui->addedMatTable->item(i, 3)->text().replace(",", ".").replace("-", "0"));
                prepQuery = prepQuery.arg(i);

                if (!db->execQuery(prepQuery)) {
                    db->showError(this);
                    db->rollbackTransaction();
                    return false;
                }
            }
        }
        query = "UPDATE defects SET def_quarter = '%1', def_devtype = '%2', def_kks = '%3', def_journaldesc = '%4', def_realdesc = '%5', "
                "def_stage = '%6', def_repairdesc = '%7', def_unit = '%8', def_devname = '%9', def_actionsdesc = '%10' WHERE def_id = '%11'";
        query = query.arg(ui->quarterBox->currentIndex()+1);
        query = query.arg(device.type).arg(device.kks).arg(ui->journalDefectEdit->text());
        query = query.arg(ui->defectEdit->text()).arg(ui->stageBox->currentIndex()).arg(ui->repairEdit->text());
        query = query.arg(device.unitId);
        query = query.arg(device.name);
        query = query.arg(ui->actionsTextEdit->toPlainText());
        query = query.arg(defId);

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }
    }
    else {
        query = "INSERT INTO defects (def_quarter, def_devtype, def_kks, def_journaldesc, def_realdesc, def_stage, def_repairdesc, def_unit, def_devname, def_actionsdesc) "
                "VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10')";
        query = query.arg(ui->quarterBox->currentIndex() + 1);
        query = query.arg(device.type).arg(device.kks).arg(ui->journalDefectEdit->text()).arg(ui->defectEdit->text());
        query = query.arg(ui->stageBox->currentIndex()).arg(ui->repairEdit->text());
        query = query.arg(device.unitId);
        query = query.arg(device.name);
        query = query.arg(ui->actionsTextEdit->toPlainText());

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }

        defId = db->lastInsertId().toString();

        query = "INSERT INTO defadditionalmats (dam_defect, dam_material, dam_oesn, dam_count) VALUES ('%1', '%2', '%3','%4')";

        for (int i=0; i<ui->addedMatTable->rowCount(); i++)
        {
            prepQuery = query.arg(defId).arg(ui->addedMatTable->item(i, 0)->text());
            prepQuery = prepQuery.arg(ui->addedMatTable->item(i, 2)->text().replace(",", ".").replace("-", "0"));
            prepQuery = prepQuery.arg(ui->addedMatTable->item(i, 3)->text().replace(",", ".").replace("-", "0"));
            if (!db->execQuery(prepQuery)) {
                db->showError(this);
                db->rollbackTransaction();
                return false;
            }
        }
    }
    db->commitTransaction();
    emit defectSaved();
    return true;
}

void DefectForm::okClicked()
{
    if (ui->saveDefectButton->isEnabled()) saveDefectClicked();
    if (ui->saveRepairButton->isEnabled()) saveRepairClicked();
    if (saveDefect()) {
        close();
    }
}

void DefectForm::cancelClicked()
{
    close();
}

void DefectForm::setNormativeForm(NormativeForm *nf)
{
    this->nf = nf;
    connect(nf, &NormativeForm::saved, this, &DefectForm::normativeSaved);
}

void DefectForm::oesnClicked()
{
    nf->show();
    nf->setDevice(device.type);
    nf->setWorkType("ТР");
}

void DefectForm::cellDoubleClicked(int row, int column)
{
    if (column < 2) return;

    FieldEditor *fe = new FieldEditor(ui->addedMatTable->viewport());
    fe->setType(EREAL);
    fe->setCell(row, column);
    fe->setGeometry(ui->addedMatTable->visualItemRect(ui->addedMatTable->item(row, column)));
    fe->setText(ui->addedMatTable->item(row, column)->text());
    connect(fe, &FieldEditor::acceptInput, this, &DefectForm::inputAccepted);
    connect(fe, &FieldEditor::rejectInput, this, &DefectForm::inputRejected);
    fe->show();
    fe->selectAll();
    fe->setFocus();
}

void DefectForm::inputAccepted(FieldEditor *editor)
{
    ui->addedMatTable->item(editor->getRow(), editor->getColumn())->setText(editor->text());
    editor->hide();
    editor->deleteLater();
    matsChanged = true;
}

void DefectForm::inputRejected(FieldEditor *editor)
{
    editor->hide();
    editor->deleteLater();
}

void DefectForm::normativeSaved(QString device, QString workType)
{
    int currStage;
    if ((!isVisible()) || (workType != "ТР")) return;
    if (device != this->device.type) return;
    updateActions();
    currStage = ui->stageBox->currentIndex();
    updateStages();
    if (currStage > ui->stageBox->count()-1) currStage = ui->stageBox->count()-1;
    if (ui->stageBox->count())
        ui->stageBox->setCurrentIndex(currStage);
}

void DefectForm::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F4)
        deviceButtonClicked();
    if (event->key() == Qt::Key_F5)
        if (ui->oesnButton->isEnabled())
            oesnClicked();
    if (event->key() == Qt::Key_Escape)
        close();
    QWidget::keyPressEvent(event);
}

void DefectForm::fillButtonClicked()
{
    QList<QStringList> oesnMatsList;
    QStringList mat;
    QStringList addedMats;
    QString query = "SELECT nm_material, mat_name, nm_count FROM normativmat AS nm LEFT JOIN materials AS mat ON nm.nm_material = mat.mat_id "
            "WHERE nm_dev = '%1' AND nm_worktype = 'ТР'";
    query = query.arg(device.type);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord()) {
        mat.clear();
        for (int i=0; i<3; i++)
        {
          mat.append(db->fetchValue(i).toString());
        }
        oesnMatsList.append(mat);
    }

    for (int i=0; i<ui->addedMatTable->rowCount(); i++)
    {
        addedMats.append(ui->addedMatTable->item(i,0)->text());
    }

    for (int i=oesnMatsList.count()-1; i>=0; i--)
    {
        if (addedMats.contains(oesnMatsList[i][0])) continue;
        ui->addedMatTable->insertRow(0);
        for (int j=0; j<4; j++)
        {
            if (j == 3) {
                ui->addedMatTable->setItem(0, 3, new QTableWidgetItem(oesnMatsList[i][2]));
                continue;
            }
            ui->addedMatTable->setItem(0, j, new QTableWidgetItem(oesnMatsList[i][j]));
        }
    }
    ui->addedMatTable->resizeColumnsToContents();
    updateMaterials();
    matsChanged = true;
}

void DefectForm::defectTextChanged()
{
    if (!ui->saveDefectButton->isEnabled()) ui->saveDefectButton->setEnabled(true);
    updateActionsDesc();
}

void DefectForm::repairTextChanged()
{
    if (!ui->saveRepairButton->isEnabled()) ui->saveRepairButton->setEnabled(true);
    updateActionsDesc();
}

void DefectForm::addedMatsRowMoved()
{
    matsChanged = true;
}
