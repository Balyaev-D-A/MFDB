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
    ui->addedMatTable->setMovableRows(true);
    connect(ui->selectDeviceButton, &QToolButton::clicked, this, &KRForm::selectDeviceClicked);
    connect(ui->addMaterialButton, &QToolButton::clicked, this, &KRForm::addMaterialClicked);
    connect(ui->removeMaterialButton, &QToolButton::clicked, this, &KRForm::removeMaterialClicked);
    connect(ui->cancelButton, &QToolButton::clicked, this, &KRForm::cancelClicked);
    connect(ui->okButton, &QToolButton::clicked, this, &KRForm::okClicked);
    connect(ui->addedMatTable, &DragDropTable::itemDroped, this, &KRForm::addMaterialClicked);
    connect(ui->materialTable, &DragDropTable::itemDroped, this, &KRForm::removeMaterialClicked);
    connect(ui->oesnButton, &QToolButton::clicked, this, &KRForm::oesnClicked);
    connect(ui->addedMatTable, &DragDropTable::cellDoubleClicked, this, &KRForm::cellDoubleClicked);
    connect(ui->fillButton, &QToolButton::clicked, this, &KRForm::fillButtonClicked);
    connect(ui->addedMatTable, &DragDropTable::rowMoved, this, &KRForm::addedMatsRowMoved);
    ui->materialTable->sortByColumn(1, Qt::AscendingOrder);
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
    ui->actionsEdit->clear();
    ui->oesnButton->setDisabled(true);
    ui->materialTable->setDisabled(true);
    ui->addedMatTable->setDisabled(true);
    ui->addMaterialButton->setDisabled(true);
    ui->removeMaterialButton->setDisabled(true);
    ui->okButton->setDisabled(true);
    ui->fillButton->setDisabled(true);
    ui->actionsEdit->setDisabled(true);
    while (ui->addedMatTable->rowCount()) ui->addedMatTable->removeRow(0);
    updateMaterials();
}

void KRForm::editKR(QString KRId)
{
    QString query;

    setWindowTitle("Редактировать капитальный ремонт");

    this->KRId = KRId;
    matsChanged = false;
    ui->materialTable->setDisabled(false);
    ui->addedMatTable->setDisabled(false);
    ui->addMaterialButton->setDisabled(false);
    ui->removeMaterialButton->setDisabled(false);
    ui->okButton->setDisabled(false);
    ui->oesnButton->setDisabled(false);
    ui->fillButton->setDisabled(false);
    ui->actionsEdit->setDisabled(false);
    query = "SELECT kr_sched, sch_name, sch_type, sch_kks, kr_actions FROM kaprepairs AS kr "
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
        if (!db->fetchValue(4).toString().simplified().isEmpty())
            ui->actionsEdit->document()->setPlainText(db->fetchValue(4).toString());
        else
            loadActionsFromOESN();
        ui->oesnButton->setDisabled(false);
    }
    else
        return;
    updateAddedMats();
    updateMaterials();
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

            query = "INSERT INTO kradditionalmats (kam_kr, kam_material, kam_oesn, kam_count) VALUES ('%1', '%2', '%3', '%4')";

            for (int i=0; i<ui->addedMatTable->rowCount(); i++)
            {
                prepQuery = query.arg(KRId).arg(ui->addedMatTable->item(i, 0)->text());
                prepQuery = prepQuery.arg(ui->addedMatTable->item(i, 2)->text().replace(",", ".").replace("-", "0"));
                prepQuery = prepQuery.arg(ui->addedMatTable->item(i, 3)->text().replace(",", ".").replace("-", "0"));

                if (!db->execQuery(prepQuery)) {
                    db->showError(this);
                    db->rollbackTransaction();
                    return false;
                }
            }
        }

        query = "UPDATE kaprepairs SET kr_sched = '%1', kr_actions = '%2' WHERE kr_id = '%3'";
        query = query.arg(selectedSched).arg(ui->actionsEdit->document()->toPlainText()).arg(KRId);

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }
    }
    else {
        query = "INSERT INTO kaprepairs (kr_sched, kr_actions) VALUES ('%1', '%2')";
        query = query.arg(selectedSched).arg(ui->actionsEdit->document()->toPlainText());

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }

        KRId = db->lastInsertId().toString();

        query = "INSERT INTO kradditionalmats (kam_kr, kam_material, kam_oesn, kam_count, kam_order) VALUES ('%1', '%2', '%3', '%4', '%5')";

        for (int i=0; i<ui->addedMatTable->rowCount(); i++)
        {

            prepQuery = query.arg(KRId).arg(ui->addedMatTable->item(i, 0)->text());
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
    ui->fillButton->setDisabled(false);
    ui->actionsEdit->setDisabled(false);
    updateAddedMats();
    updateMaterials();
    loadActionsFromOESN();
}

void KRForm::oesnClicked()
{
    nf->show();
    nf->setDevice(selectedDevice);
    nf->setWorkType("КР");
}

void KRForm::updateAddedMats()
{
    int curRow;
    QString query;
    while (ui->addedMatTable->rowCount() > 0) ui->addedMatTable->removeRow(0);

    query = "SELECT kam_material, mat_name, kam_oesn, kam_count FROM kradditionalmats AS k "
            "LEFT JOIN materials AS m ON k.kam_material = m.mat_id "
            "WHERE kam_kr = '%1' ORDER BY kam_order";
    query = query.arg(KRId);

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
    updateMaterials();
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
    ui->addedMatTable->item(editor->getRow(), editor->getColumn())->setText(editor->text().simplified());
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

void KRForm::fillButtonClicked()
{
    QList<QStringList> oesnMatsList;
    QStringList mat;
    QStringList addedMats;
    QString query = "SELECT nm_material, mat_name, nm_count FROM normativmat AS nm LEFT JOIN materials AS mat ON nm.nm_material = mat.mat_id "
            "WHERE nm_dev = '%1' AND nm_worktype = 'КР'";
    query = query.arg(selectedDevice);

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

void KRForm::loadActionsFromOESN()
{
    QString query = "SELECT na_actions FROM normativactions WHERE na_dev = '%1' AND na_worktype = 'КР'";
    query = query.arg(selectedDevice);

    if (!db->execQuery(query))
    {
        db->showError(this);
        return;
    }

    if (db->nextRecord())
        ui->actionsEdit->document()->setPlainText(db->fetchValue(0).toString());
    else
        ui->actionsEdit->document()->setPlainText("");
}

void KRForm::addedMatsRowMoved()
{
    matsChanged = true;
}

void KRForm::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    QWidget::keyPressEvent(event);
}
