#include "krform.h"
#include "ui_krform.h"

KRForm::KRForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KRForm)
{
    setWindowFlag(Qt::Window, true);
    ui->setupUi(this);

    ui->addedMatTable->setAcceptFrom(ui->materialTable);
    ui->materialTable->setAcceptFrom(ui->addedMatTable);
    ui->addedMatTable->hideColumn(0);
    ui->materialTable->hideColumn(0);
    connect(ui->selectDeviceButton, &QToolButton::clicked, this, &KRForm::selectDeviceClicked);
    connect(ui->addMaterialButton, &QToolButton::clicked, this, &KRForm::addMaterialClicked);
    connect(ui->removeMaterialButton, &QToolButton::clicked, this, &KRForm::removeMaterialClicked);
    connect(ui->cancelButton, &QToolButton::clicked, this, &KRForm::cancelClicked);
    connect(ui->okButton, &QToolButton::clicked, this, &KRForm::okClicked);
    connect(ui->addedMatTable, &DragDropTable::itemDroped, this, &KRForm::addMaterialClicked);
    connect(ui->materialTable, &DragDropTable::itemDroped, this, &KRForm::removeMaterialClicked);
    connect(ui->oesnButton, &QToolButton::clicked, this, &KRForm::oesnClicked);
    connect(ui->addedMatTable, &DragDropTable::cellDoubleClicked, this, &KRForm::cellDoubleClicked);
}

KRForm::~KRForm()
{
    delete ui;
}

void KRForm::setDatabase(Database *db)
{
    this->db = db;
}

void KRForm::setNormativeForm(NormativeForm *nf)
{
    this->nf = nf;
    connect(nf, &NormativeForm::saved, this, &KRForm::normativeSaved);
}

void KRForm::newKR()
{
    setWindowTitle("Новый капитальный ремонт");
    KRId = "0";
    selectedSched = "0";
    matsChanged = false;
    ui->deviceEdit->clear();
    ui->oesnButton->setDisabled(true);
    ui->materialTable->setDisabled(true);
    ui->addedMatTable->setDisabled(true);
    ui->addMaterialButton->setDisabled(true);
    ui->removeMaterialButton->setDisabled(true);
    ui->okButton->setDisabled(true);
    while (ui->addedMatTable->rowCount()) ui->addedMatTable->removeRow(0);
    updateMaterials();
}

void KRForm::editKR(QString KRId)
{
    QString query;

    setWindowTitle("Редактировать капитальный ремонт");

    this->KRId = KRId;
    matsChanged = false;
    ui->materialTable->setDisabled(true);
    ui->addedMatTable->setDisabled(true);
    ui->addMaterialButton->setDisabled(true);
    ui->removeMaterialButton->setDisabled(true);
    ui->okButton->setDisabled(true);
    ui->oesnButton->setDisabled(true);
    query = "SELECT kr_sched, sch_name, sch_type, sch_kks FROM kaprepairs AS kr "
            "LEFT JOIN schedule AS sch ON kr.kr_sched = sch.sch_id "
            "WHERE kr_id = '%1'";
    query = query.arg(KRId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()) {
        selectedSched = db->fetchValue(0).toString();
        selectedDevice = db->fetchValue(2).toString();
        ui->deviceEdit->setText(db->fetchValue(1).toString() + " " + db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
        ui->oesnButton->setDisabled(false);
    }
    else
        return;
    updateAddedMats();
    updateMaterials();
    ui->materialTable->setDisabled(false);
    ui->addedMatTable->setDisabled(false);
    ui->addMaterialButton->setDisabled(false);
    ui->removeMaterialButton->setDisabled(false);
    ui->okButton->setDisabled(false);
}

void KRForm::selectDeviceClicked()
{
    KRSelectorForm *ksf = new KRSelectorForm();
    ksf->setDatabase(db);
    connect(ksf, &KRSelectorForm::closed, this, &KRForm::selectorClosed);
    connect(ksf, &KRSelectorForm::selected, this, &KRForm::deviceSelected);
    ksf->show();
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
    ui->addedMatTable->insertRow(ui->addedMatTable->rowCount());
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 0, new QTableWidgetItem(
                                   ui->materialTable->item(ui->materialTable->currentRow(), 0)->text()));
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 1, new QTableWidgetItem(
                                   ui->materialTable->item(ui->materialTable->currentRow(), 1)->text()));
    ui->materialTable->removeRow(ui->materialTable->currentRow());
    ui->addedMatTable->resizeColumnsToContents();
    matsChanged = true;
}

void KRForm::removeMaterialClicked()
{
    if (ui->addedMatTable->currentRow() < 0) return;
    if (ui->addedMatTable->isPersistentRow(ui->addedMatTable->currentRow())) return;
    ui->addedMatTable->removeRow(ui->addedMatTable->currentRow());
    matsChanged = true;
    updateMaterials();
}

bool KRForm::saveKR()
{
    QString query;
    QString prepQuery;
    float oesn, real;
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
                if (ui->addedMatTable->isPersistentRow(i)){
                    oesn = ui->addedMatTable->item(i, 2)->text().toFloat();
                    real = ui->addedMatTable->item(i, 3)->text().toFloat();
                    if (oesn == real) continue;
                }
                prepQuery = query.arg(KRId).arg(ui->addedMatTable->item(i, 0)->text()).arg(ui->addedMatTable->item(i, 3)->text());

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

        query = "INSERT INTO kradditionalmats (kam_kr, kam_material, kam_count) VALUES ('%1', '%2', '%3')";

        for (int i=0; i<ui->addedMatTable->rowCount(); i++)
        {
            if (ui->addedMatTable->isPersistentRow(i)){
                oesn = ui->addedMatTable->item(i, 2)->text().toFloat();
                real = ui->addedMatTable->item(i, 3)->text().toFloat();
                if (oesn == real) continue;
            }
            prepQuery = query.arg(KRId).arg(ui->addedMatTable->item(i, 0)->text()).arg(ui->addedMatTable->item(i, 3)->text());

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

void KRForm::selectorClosed(KRSelectorForm *sender)
{
    sender->deleteLater();
}

void KRForm::deviceSelected(const KRDevice &device)
{
    selectedSched = device.sched;
    selectedDevice = device.type;
    ui->deviceEdit->setText(device.device + " " + device.type + " " + device.kks);
    ui->materialTable->setDisabled(false);
    ui->addedMatTable->setDisabled(false);
    ui->addMaterialButton->setDisabled(false);
    ui->removeMaterialButton->setDisabled(false);
    ui->okButton->setDisabled(false);
    ui->oesnButton->setDisabled(false);
    updateAddedMats();
    updateMaterials();
}

void KRForm::oesnClicked()
{
    nf->show();
    nf->setDevice(selectedDevice);
    nf->setWorkType("КР");
}

void KRForm::updateAddedMats()
{
    int foundRow, curRow;
    QString query = "SELECT nm_material, mat_name, nm_count FROM normativmat AS nm LEFT JOIN materials AS mat ON nm.nm_material = mat.mat_id "
            "WHERE nm_dev = '%1' AND nm_worktype = 'КР'";
    query = query.arg(selectedDevice);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->addedMatTable->rowCount() > 0) ui->addedMatTable->removeRow(0);
    ui->addedMatTable->clearPersistentRows();

    for (int i=0; db->nextRecord(); i++)
    {
        ui->addedMatTable->insertRow(i);
        ui->addedMatTable->setItem(i, 0, new QTableWidgetItem(db->fetchValue(0).toString()));
        ui->addedMatTable->setItem(i, 1, new QTableWidgetItem(db->fetchValue(1).toString()));
        ui->addedMatTable->setItem(i, 2, new QTableWidgetItem(db->fetchValue(2).toString()));
        ui->addedMatTable->setItem(i, 3, new QTableWidgetItem(db->fetchValue(2).toString()));
        ui->addedMatTable->setPersistentRow(i);
    }

    query = "SELECT kam_material, mat_name, kam_count FROM kradditionalmats AS k "
            "LEFT JOIN materials AS m ON k.kam_material = m.mat_id "
            "WHERE kam_kr = '%1'";
    query = query.arg(KRId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    while (db->nextRecord())
    {
        foundRow = -1;
        for (int i=0; i<ui->addedMatTable->rowCount(); i++)
            if (ui->addedMatTable->item(i, 0)->text() == db->fetchValue(0).toString())
                foundRow = i;

        if (foundRow < 0) {
            curRow = ui->addedMatTable->rowCount();
            ui->addedMatTable->insertRow(curRow);
            ui->addedMatTable->setItem(curRow, 0, new QTableWidgetItem(db->fetchValue(0).toString()));
            ui->addedMatTable->setItem(curRow, 1, new QTableWidgetItem(db->fetchValue(1).toString()));
            ui->addedMatTable->setItem(curRow, 2, new QTableWidgetItem("-"));
            ui->addedMatTable->setItem(curRow, 3, new QTableWidgetItem(db->fetchValue(2).toString()));
        }
        else {
            ui->addedMatTable->item(foundRow ,3)->setText(db->fetchValue(2).toString());
        }
    }
    ui->addedMatTable->resizeColumnsToContents();
}

void KRForm::cellDoubleClicked(int row, int column)
{
    if (column != 3) return;

    FieldEditor *fe = new FieldEditor(ui->addedMatTable->viewport());
    fe->setType(EREAL);
    fe->setCell(row, column);
    fe->setGeometry(ui->addedMatTable->visualItemRect(ui->addedMatTable->item(row, column)));
    fe->setText(ui->addedMatTable->item(row, column)->text());
    connect(fe, &FieldEditor::acceptInput, this, &KRForm::inputAccepted);
    connect(fe, &FieldEditor::rejectInput, this, &KRForm::inputRejected);
    fe->show();
    fe->selectAll();
    fe->setFocus();
}

void KRForm::inputAccepted(FieldEditor *editor)
{
    ui->addedMatTable->item(editor->getRow(), editor->getColumn())->setText(editor->text());
    editor->hide();
    editor->deleteLater();
    matsChanged = true;
}

void KRForm::inputRejected(FieldEditor *editor)
{
    editor->hide();
    editor->deleteLater();
}

void KRForm::normativeSaved(QString device, QString workType)
{
    if ((!isVisible()) || (workType != "КР")) return;
    if (device != selectedDevice) return;
    updateAddedMats();
    updateMaterials();
}
