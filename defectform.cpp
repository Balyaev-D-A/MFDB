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
    connect(ui->nextDefectButton, &QToolButton::clicked, this, &DefectForm::nextDefectClicked);
    connect(ui->prevDefectButton, &QToolButton::clicked, this, &DefectForm::prevDefectClicked);
    connect(ui->nextRepairButton, &QToolButton::clicked, this, &DefectForm::nextRepairClicked);
    connect(ui->prevRepairButton, &QToolButton::clicked, this, &DefectForm::prevRepairClicked);

    ui->journalDefectEdit->setDisabled(true);
    ui->materialTable->setDisabled(true);
    ui->addedMatTable->setDisabled(true);
    ui->addMaterialButton->setDisabled(true);
    ui->removeMaterialButton->setDisabled(true);
    ui->buttonGroup->setId(ui->externRevievRB, EXTERNREVIEW);
    ui->buttonGroup->setId(ui->externConnRB, EXTERNCONN);
    ui->buttonGroup->setId(ui->internConnRB, INTERNCONN);
    ui->buttonGroup->setId(ui->electricParRB, ELECTRICPAR);
    updateDefectText();
    updateRepairText();
}

DefectForm::~DefectForm()
{
    delete ui;
}

void DefectForm::setDatabase(Database *db)
{
    this->db = db;
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
    ui->deviceEdit->setText(device.devName + " " + device.devType + " " + device.devKKS);
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
    query = query.arg(device.devType);
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
        if (device.devName == "")
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
    query = query.arg(device.devType);

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
}

void DefectForm::deleteDefectClicked()
{
    QString query = "DELETE FROM defrealdescs WHERE drd_id = '%1'";
    query = query.arg(defectList[currentDefect].id);

    if (!)
}
