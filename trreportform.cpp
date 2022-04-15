#include <QMessageBox>

#include "trreportform.h"
#include "ui_trreportform.h"

TRReportForm::TRReportForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::TRReportForm)
{
    ui->setupUi(this);
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setCurrentIndex(0);
    ui->trTable->setAcceptFrom(ui->selectedTRTable);
    ui->selectedTRTable->setAcceptFrom(ui->trTable);
    ui->trTable->hideColumn(0);
    ui->selectedTRTable->hideColumn(0);
    ui->begDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setDate(QDate::currentDate());
    ui->signersTable->hideColumn(0);
    ui->ownerEdit->setAcceptFrom(ui->signersTable);
    ui->member1Edit->setAcceptFrom(ui->signersTable);
    ui->member2Edit->setAcceptFrom(ui->signersTable);
    ui->member3Edit->setAcceptFrom(ui->signersTable);
    ui->member4Edit->setAcceptFrom(ui->signersTable);
    ui->member5Edit->setAcceptFrom(ui->signersTable);
    ui->repairerEdit->setAcceptFrom(ui->signersTable);
    ui->chiefEdit->setAcceptFrom(ui->signersTable);
    ui->member4Edit->setStyleSheet("background: rgb(255, 255, 0);");
    ui->member5Edit->setStyleSheet("background: rgb(255, 255, 0);");
    connect(ui->unitBox, &QComboBox::currentTextChanged, this, &TRReportForm::unitChanged);
    connect(ui->addButton, &QToolButton::clicked, this, &TRReportForm::addButtonClicked);
    connect(ui->removeButton, &QToolButton::clicked, this, &TRReportForm::removeButtonClicked);
    connect(ui->trTable, &DragDropTable::itemDroped, this, &TRReportForm::removeButtonClicked);
    connect(ui->selectedTRTable, &DragDropTable::itemDroped, this, &TRReportForm::addButtonClicked);
    connect(ui->addAllButton, &QToolButton::clicked, this, &TRReportForm::addAllButtonClicked);
    connect(ui->removeAllButton, &QToolButton::clicked, this, &TRReportForm::removeAllButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &TRReportForm::close);
    connect(ui->nextButton, &QPushButton::clicked, this, &TRReportForm::nextButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &TRReportForm::backButtonClicked);
    connect(ui->ownerEdit, &DragDropEdit::itemDroped, this, &TRReportForm::ownerDroped);
    connect(ui->member1Edit, &DragDropEdit::itemDroped, this, &TRReportForm::member1Droped);
    connect(ui->member2Edit, &DragDropEdit::itemDroped, this, &TRReportForm::member2Droped);
    connect(ui->member3Edit, &DragDropEdit::itemDroped, this, &TRReportForm::member3Droped);
    connect(ui->member4Edit, &DragDropEdit::itemDroped, this, &TRReportForm::member4Droped);
    connect(ui->member5Edit, &DragDropEdit::itemDroped, this, &TRReportForm::member5Droped);
    connect(ui->repairerEdit, &DragDropEdit::itemDroped, this, &TRReportForm::repairerDroped);
    connect(ui->chiefEdit, &DragDropEdit::itemDroped, this, &TRReportForm::chiefDroped);
    connect(ui->doneButton, &QPushButton::clicked, this, &TRReportForm::doneButtonClicked);
    connect(ui->ownerClearButton, &QToolButton::clicked, this, &TRReportForm::ownerClearClicked);
    connect(ui->member1ClearButton, &QToolButton::clicked, this, &TRReportForm::member1ClearClicked);
    connect(ui->member2ClearButton, &QToolButton::clicked, this, &TRReportForm::member2ClearClicked);
    connect(ui->member3ClearButton, &QToolButton::clicked, this, &TRReportForm::member3ClearClicked);
    connect(ui->member4ClearButton, &QToolButton::clicked, this, &TRReportForm::member4ClearClicked);
    connect(ui->member5ClearButton, &QToolButton::clicked, this, &TRReportForm::member5ClearClicked);
    connect(ui->repairerClearButton, &QToolButton::clicked, this, &TRReportForm::repairerClearClicked);
    connect(ui->chiefClearButton, &QToolButton::clicked, this, &TRReportForm::chiefClearClicked);
}

TRReportForm::~TRReportForm()
{
    delete ui;
}

void TRReportForm::setDatabase(Database *db)
{
    this->db = db;
}

void TRReportForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fillUnitBox();
    updateTRTable();
    updateSignersTable();
}

void TRReportForm::editReport(QString Id)
{
    int curRow;
    QString query;

    reportId = Id;

    query = "SELECT trr_desc, trr_unit, trr_planbeg, trr_planend, trr_date, trr_docnum FROM trreports WHERE trr_id = '%1'";

    query = query.arg(reportId);
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()) {
        ui->descEdit->setText(db->fetchValue(0).toString());
        unitId = db->fetchValue(1).toInt();
        ui->begDateEdit->setDate(QDate::fromString(db->fetchValue(2).toString(), "dd.MM.yyyy"));
        ui->endDateEdit->setDate(QDate::fromString(db->fetchValue(3).toString(), "dd.MM.yyyy"));
        ui->dateEdit->setDate(QDate::fromString(db->fetchValue(4).toString(), "dd.MM.yyyy"));
        ui->docNumEdit->setText(db->fetchValue(5).toString());
    }

    query = "SELECT trw_work, def_devtype, def_kks, def_begdate, def_enddate FROM trrworks "
            "LEFT JOIN defects ON trw_work = def_id "
            "WHERE trw_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    ui->selectedTRTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        curRow = ui->selectedTRTable->rowCount();
        ui->selectedTRTable->insertRow(curRow);
        for (int i=0; i<5; i++)
        {
            ui->selectedTRTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->selectedTRTable->setSortingEnabled(true);

    query = "SELECT trs_signer, trs_role, sig_name, sig_loc FROM trsigners "
            "LEFT JOIN signers ON trs_signer = sig_id "
            "WHERE trs_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    while (db->nextRecord())
    {
        switch (db->fetchValue(1).toInt()) {
        case TRSOWNER:
            signers.ownerId = db->fetchValue(0).toString();
            ui->ownerEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case TRSMEMBER1:
            signers.member1Id = db->fetchValue(0).toString();
            ui->member1Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case TRSMEMBER2:
            signers.member2Id = db->fetchValue(0).toString();
            ui->member2Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case TRSMEMBER3:
            signers.member3Id = db->fetchValue(0).toString();
            ui->member3Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case TRSMEMBER4:
            signers.member4Id = db->fetchValue(0).toString();
            ui->member4Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case TRSMEMBER5:
            signers.member5Id = db->fetchValue(0).toString();
            ui->member5Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case TRSREPAIRER:
            signers.repairerId = db->fetchValue(0).toString();
            ui->repairerEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case TRSCHIEF:
            signers.chiefId = db->fetchValue(0).toString();
            ui->chiefEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        }
    }
}

void TRReportForm::fillUnitBox()
{
    QString query = "SELECT unit_id, unit_name FROM units ORDER BY unit_name";

    ui->unitBox->blockSignals(true);
    while(ui->unitBox->count()) ui->unitBox->removeItem(0);
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    while (db->nextRecord())
    {
        ui->unitBox->addItem(db->fetchValue(1).toString(), db->fetchValue(0));
    }
    if (unitId != -1) ui->unitBox->setCurrentIndex(ui->unitBox->findData(unitId));
    ui->unitBox->blockSignals(false);
}

void TRReportForm::updateTRTable()
{
    int curRow;
    QStringList usedIds;
    QStringList inReportIds;
    QString query = "SELECT trw_work FROM trrworks";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord())
        inReportIds.append(db->fetchValue(0).toString());

    query = "SELECT def_id, def_devtype, def_kks, def_begdate, def_enddate FROM defects "
                    "WHERE def_unit = '%1' AND def_begdate <> 'NULL' AND def_enddate <> 'NULL' AND def_num <> 'NULL'";
    query = query.arg(ui->unitBox->currentData().toString());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    for (int i=0; i<ui->selectedTRTable->rowCount(); i++)
    {
        usedIds.append(ui->selectedTRTable->item(i, 0)->text());
    }

    while (ui->trTable->rowCount()) ui->trTable->removeRow(0);

    ui->trTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        if (usedIds.contains(db->fetchValue(0).toString())) continue;
        if (inReportIds.contains(db->fetchValue(0).toString())) continue;
        curRow = ui->trTable->rowCount();
        ui->trTable->insertRow(curRow);
        for (int i=0; i<5; i++)
        {
            ui->trTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->trTable->setSortingEnabled(true);
}

void TRReportForm::updateSignersTable()
{
    int curRow;
    QString sigId;
    QString query;

    query = "SELECT sig_id, sig_name, sig_loc FROM signers WHERE sig_hidden = 'FALSE'";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->signersTable->rowCount()) ui->signersTable->removeRow(0);

    while (db->nextRecord())
    {
        sigId = db->fetchValue(0).toString();
        curRow = ui->signersTable->rowCount();
        if (sigId == signers.ownerId) continue;
        if (sigId == signers.member1Id) continue;
        if (sigId == signers.member2Id) continue;
        if (sigId == signers.member3Id) continue;
        if (sigId == signers.member4Id) continue;
        if (sigId == signers.member5Id) continue;
        if (sigId == signers.repairerId) continue;
        if (sigId == signers.chiefId) continue;
        ui->signersTable->insertRow(curRow);
        for (int i=0; i<3; i++)
        {
            ui->signersTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->signersTable->resizeColumnsToContents();
}
void TRReportForm::addButtonClicked()
{
    int curKRRow, curSelRow;
    if (!ui->trTable->currentItem()) return;
    curKRRow = ui->trTable->currentRow();
    curSelRow = ui->selectedTRTable->rowCount();
    ui->selectedTRTable->insertRow(curSelRow);
    for (int i=0; i<ui->trTable->columnCount(); i++)
    {
        ui->selectedTRTable->setItem(curSelRow, i, new QTableWidgetItem(
                                         ui->trTable->item(curKRRow, i)->text()));
    }
    ui->trTable->removeRow(curKRRow);
}

void TRReportForm::removeButtonClicked()
{
    if (!ui->selectedTRTable->currentItem()) return;
    ui->selectedTRTable->removeRow(ui->selectedTRTable->currentRow());
    updateTRTable();
}

void TRReportForm::unitChanged()
{
    while (ui->selectedTRTable->rowCount()) ui->selectedTRTable->removeRow(0);
    updateTRTable();
}

void TRReportForm::nextButtonClicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void TRReportForm::backButtonClicked()
{
    ui->tabWidget->setCurrentIndex(0);
}

void TRReportForm::ownerDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.ownerId = ui->signersTable->item(curRow, 0)->text();
    ui->ownerEdit->setText(text);
    updateSignersTable();
}

void TRReportForm::member1Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member1Id = ui->signersTable->item(curRow, 0)->text();
    ui->member1Edit->setText(text);
    updateSignersTable();
}

void TRReportForm::member2Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member2Id = ui->signersTable->item(curRow, 0)->text();
    ui->member2Edit->setText(text);
    updateSignersTable();
}

void TRReportForm::member3Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member3Id = ui->signersTable->item(curRow, 0)->text();
    ui->member3Edit->setText(text);
    updateSignersTable();
}

void TRReportForm::member4Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member4Id = ui->signersTable->item(curRow, 0)->text();
    ui->member4Edit->setText(text);
    updateSignersTable();
}

void TRReportForm::member5Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member5Id = ui->signersTable->item(curRow, 0)->text();
    ui->member5Edit->setText(text);
    updateSignersTable();
}
void TRReportForm::repairerDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.repairerId = ui->signersTable->item(curRow, 0)->text();
    ui->repairerEdit->setText(text);
    updateSignersTable();
}

void TRReportForm::chiefDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.chiefId = ui->signersTable->item(curRow, 0)->text();
    ui->chiefEdit->setText(text);
    updateSignersTable();
}

void TRReportForm::ownerClearClicked()
{
    ui->ownerEdit->setText("");
    signers.ownerId = "";
    updateSignersTable();
}

void TRReportForm::member1ClearClicked()
{
    ui->member1Edit->setText("");
    signers.member1Id = "";
    updateSignersTable();
}

void TRReportForm::member2ClearClicked()
{
    ui->member2Edit->setText("");
    signers.member2Id = "";
    updateSignersTable();
}

void TRReportForm::member3ClearClicked()
{
    ui->member3Edit->setText("");
    signers.member3Id = "";
    updateSignersTable();
}

void TRReportForm::member4ClearClicked()
{
    ui->member4Edit->setText("");
    signers.member4Id = "";
    updateSignersTable();
}

void TRReportForm::member5ClearClicked()
{
    ui->member5Edit->setText("");
    signers.member5Id = "";
    updateSignersTable();
}

void TRReportForm::repairerClearClicked()
{
    ui->repairerEdit->setText("");
    signers.repairerId = "";
    updateSignersTable();
}

void TRReportForm::chiefClearClicked()
{
    ui->chiefEdit->setText("");
    signers.chiefId = "";
    updateSignersTable();
}

bool TRReportForm::checkFilling()
{
    if (ui->descEdit->text().isEmpty()) {
        showErrorMessage("Не заполнено описание отчета.");
        return false;
    }
    if (ui->docNumEdit->text().isEmpty()) {
        showErrorMessage("Не заполнен номер документа.");
        return false;
    }
    if (!ui->selectedTRTable->rowCount()) {
        showErrorMessage("Не выбрано ни одной работы для отчета.");
        return false;
    }
    if (ui->ownerEdit->text().isEmpty()) {
        showErrorMessage("Не выбран подписант владельца оборудования.");
        return false;
    }
    if (ui->member1Edit->text().isEmpty()) {
        showErrorMessage("Не выбран один из членов комиссии.");
        return false;
    }
    if (ui->member2Edit->text().isEmpty()) {
        showErrorMessage("Не выбран один из членов комиссии.");
        return false;
    }
    if (ui->member3Edit->text().isEmpty()) {
        showErrorMessage("Не выбран один из членов комиссии.");
        return false;
    }
    if (ui->repairerEdit->text().isEmpty()) {
        showErrorMessage("Не выбран представитель цеха ответственного за ремонт.");
        return false;
    }
    if (ui->chiefEdit->text().isEmpty()) {
        showErrorMessage("Не выбран руководитель работ по ремонту оборудования.");
    }
    return true;
}

void TRReportForm::showErrorMessage(QString message)
{
    QMessageBox::critical(this, "Ошибка!", message);
}

void TRReportForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed(this);
}

void TRReportForm::doneButtonClicked()
{
    QString query, prepQuery;
    QStringList queryList;
    if (!checkFilling()) return;
    db->startTransaction();
    if (reportId == "0") {
        query = "INSERT INTO trreports (trr_desc, trr_unit, trr_planbeg, trr_planend, trr_date, trr_docnum) "
                "VALUES ('%1', '%2', '%3', '%4', '%5', '%6')";
        query = query.arg(ui->descEdit->text());
        query = query.arg(ui->unitBox->currentData().toString());
        query = query.arg(ui->begDateEdit->text());
        query = query.arg(ui->endDateEdit->text());
        query = query.arg(ui->dateEdit->text());
        query = query.arg(ui->docNumEdit->text());

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
        reportId = db->lastInsertId().toString();
    } else {
        query = "UPDATE trreports SET trr_desc = '%1', trr_unit = '%2', trr_planbeg = '%3', trr_planend = '%4', trr_date = '%5',  trr_docnum = '%6' "
                "WHERE trr_id = '%7'";
        query = query.arg(ui->descEdit->text());
        query = query.arg(ui->unitBox->currentData().toString());
        query = query.arg(ui->begDateEdit->text());
        query = query.arg(ui->endDateEdit->text());
        query = query.arg(ui->dateEdit->text());
        query = query.arg(ui->docNumEdit->text());
        query = query.arg(reportId);

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }

        query = "DELETE FROM trrworks WHERE trw_report = '%1'";
        query = query.arg(reportId);
        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }

        query = "DELETE FROM trsigners WHERE trs_report = '%1'";
        query = query.arg(reportId);
        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
    }

    query = "INSERT INTO trrworks (trw_work, trw_report) VALUES ('%1', '%2')";

    for (int i=0; i<ui->selectedTRTable->rowCount(); i++)
    {
        prepQuery = query.arg(ui->selectedTRTable->item(i, 0)->text()).arg(reportId);
        if (!db->execQuery(prepQuery)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
    }

    query = "INSERT INTO trsigners (trs_report, trs_signer, trs_role) VALUES ('%1', '%2', '%3')";

    queryList.clear();
    prepQuery = query.arg(reportId).arg(signers.ownerId).arg(TRSOWNER);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.member1Id).arg(TRSMEMBER1);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.member2Id).arg(TRSMEMBER2);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.member3Id).arg(TRSMEMBER3);
    queryList.append(prepQuery);
    if (signers.member4Id != "") {
        prepQuery = query.arg(reportId).arg(signers.member4Id).arg(TRSMEMBER4);
        queryList.append(prepQuery);
    }
    if (signers.member5Id != "") {
        prepQuery = query.arg(reportId).arg(signers.member5Id).arg(TRSMEMBER5);
        queryList.append(prepQuery);
    }
    prepQuery = query.arg(reportId).arg(signers.repairerId).arg(TRSREPAIRER);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.chiefId).arg(TRSCHIEF);
    queryList.append(prepQuery);

    for (int i=0; i<queryList.size(); i++)
    {
        if (!db->execQuery(queryList[i])) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
    }
    db->commitTransaction();
    emit saved();
    close();
}

void TRReportForm::addAllButtonClicked()
{
    return;
}

void TRReportForm::removeAllButtonClicked()
{
    while (ui->selectedTRTable->rowCount()) ui->selectedTRTable->removeRow(0);
    updateTRTable();
}
