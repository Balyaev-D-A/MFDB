#include "krreportform.h"
#include "ui_krreportform.h"

#include <QMessageBox>

KRReportForm::KRReportForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::KRReportForm)
{
    ui->setupUi(this);
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setCurrentIndex(0);
    ui->krTable->setAcceptFrom(ui->selectedKRTable);
    ui->selectedKRTable->setAcceptFrom(ui->krTable);
    ui->krTable->hideColumn(0);
    ui->selectedKRTable->hideColumn(0);
    ui->begDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->signersTable->hideColumn(0);
    ui->ownerEdit->setAcceptFrom(ui->signersTable);
    ui->member1Edit->setAcceptFrom(ui->signersTable);
    ui->member2Edit->setAcceptFrom(ui->signersTable);
    ui->member3Edit->setAcceptFrom(ui->signersTable);
    ui->repairerEdit->setAcceptFrom(ui->signersTable);
    ui->chiefEdit->setAcceptFrom(ui->signersTable);
    connect(ui->unitBox, &QComboBox::currentTextChanged, this, &KRReportForm::unitChanged);
    connect(ui->addButton, &QToolButton::clicked, this, &KRReportForm::addButtonClicked);
    connect(ui->removeButton, &QToolButton::clicked, this, &KRReportForm::removeButtonClicked);
    connect(ui->krTable, &DragDropTable::itemDroped, this, &KRReportForm::removeButtonClicked);
    connect(ui->selectedKRTable, &DragDropTable::itemDroped, this, &KRReportForm::addButtonClicked);
    connect(ui->addAllButton, &QToolButton::clicked, this, &KRReportForm::addAllButtonClicked);
    connect(ui->removeAllButton, &QToolButton::clicked, this, &KRReportForm::removeAllButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &KRReportForm::close);
    connect(ui->nextButton, &QPushButton::clicked, this, &KRReportForm::nextButtonClicked);
    connect(ui->ownerEdit, &DragDropEdit::itemDroped, this, &KRReportForm::ownerDroped);
    connect(ui->member1Edit, &DragDropEdit::itemDroped, this, &KRReportForm::member1Droped);
    connect(ui->member2Edit, &DragDropEdit::itemDroped, this, &KRReportForm::member2Droped);
    connect(ui->member3Edit, &DragDropEdit::itemDroped, this, &KRReportForm::member3Droped);
    connect(ui->repairerEdit, &DragDropEdit::itemDroped, this, &KRReportForm::repairerDroped);
    connect(ui->chiefEdit, &DragDropEdit::itemDroped, this, &KRReportForm::chiefDroped);
    connect(ui->doneButton, &QPushButton::clicked, this, &KRReportForm::doneButtonClicked);
}

KRReportForm::~KRReportForm()
{
    delete ui;
}

void KRReportForm::setDatabase(Database *db)
{
    this->db = db;
}

void KRReportForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fillUnitBox();
    updateKRTable();
    updateSignersTable();
}

void KRReportForm::updateKRTable()
{
    int curRow;
    QStringList usedIds;
    QString query = "SELECT kr_id, sch_type, sch_kks, kr_begdate, kr_enddate FROM kaprepairs AS kr "
                    "LEFT JOIN schedule AS sch ON kr.kr_sched = sch.sch_id "
                    "WHERE sch_unit = '%1' AND kr_begdate <> 'NULL' AND kr_enddate <> 'NULL'";
    query = query.arg(ui->unitBox->currentData().toString());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    for (int i=0; i<ui->selectedKRTable->rowCount(); i++)
    {
        usedIds.append(ui->selectedKRTable->item(i, 0)->text());
    }

    while (ui->krTable->rowCount()) ui->krTable->removeRow(0);

    ui->krTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        if (usedIds.contains(db->fetchValue(0).toString())) continue;
        curRow = ui->krTable->rowCount();
        ui->krTable->insertRow(curRow);
        for (int i=0; i<5; i++)
        {
            ui->krTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->krTable->setSortingEnabled(true);
}

void KRReportForm::fillUnitBox()
{
    QString query = "SELECT unit_id, unit_name FROM units";

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
    ui->unitBox->blockSignals(false);
}

void KRReportForm::addButtonClicked()
{
    int curKRRow, curSelRow;
    if (!ui->krTable->currentItem()) return;
    curKRRow = ui->krTable->currentRow();
    curSelRow = ui->selectedKRTable->rowCount();
    ui->selectedKRTable->insertRow(curSelRow);
    for (int i=0; i<ui->krTable->columnCount(); i++)
    {
        ui->selectedKRTable->setItem(curSelRow, i, new QTableWidgetItem(
                                         ui->krTable->item(curKRRow, i)->text()));
    }
    ui->krTable->removeRow(curKRRow);
}

void KRReportForm::removeButtonClicked()
{
    if (!ui->selectedKRTable->currentItem()) return;
    ui->selectedKRTable->removeRow(ui->selectedKRTable->currentRow());
    updateKRTable();
}

void KRReportForm::unitChanged()
{
    while (ui->selectedKRTable->rowCount()) ui->selectedKRTable->removeRow(0);
    updateKRTable();
}

void KRReportForm::addAllButtonClicked()
{
    for (int i=0; i<ui->krTable->rowCount(); i++)
    {
        ui->krTable->selectRow(i);
        addButtonClicked();
    }
}

void KRReportForm::removeAllButtonClicked()
{
    while (ui->selectedKRTable->rowCount()) ui->selectedKRTable->removeRow(0);
    updateKRTable();
}

void KRReportForm::nextButtonClicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void KRReportForm::backButtonClicked()
{
    ui->tabWidget->setCurrentIndex(0);
}

void KRReportForm::showErrorMessage(QString message)
{
    QMessageBox::critical(this, "Ошибка!", message);
}

bool KRReportForm::checkFilling()
{
    if (ui->descEdit->text().isEmpty()) {
        showErrorMessage("Не заполнено описание отчета.");
        return false;
    }
    if (ui->docNumEdit->text().isEmpty()) {
        showErrorMessage("Не заполнен номер документа.");
        return false;
    }
    if (!ui->selectedKRTable->rowCount()) {
        showErrorMessage("Не выбрано ни одной работы для отчета.");
        return false;
    }
    if (ui->ownerEdit->text().isEmpty()) {
        showErrorMessage("Не выбран подписант владельца оборудования.");
        return false;
    }
    if (ui->member1Edit->text().isEmpty()) {
        showErrorMessage("Не выбран один из членнов комиссии.");
        return false;
    }
    if (ui->member2Edit->text().isEmpty()) {
        showErrorMessage("Не выбран один из членнов комиссии.");
        return false;
    }
    if (ui->member3Edit->text().isEmpty()) {
        showErrorMessage("Не выбран один из членнов комиссии.");
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

void KRReportForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed(this);
}

void KRReportForm::updateSignersTable()
{
    int curRow;
    QString query = "SELECT sig_id, sig_name, sig_loc FROM signers WHERE sig_hidden = 'FALSE'";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->signersTable->rowCount()) ui->signersTable->removeRow(0);

    while (db->nextRecord())
    {
        curRow = ui->signersTable->rowCount();
        ui->signersTable->insertRow(curRow);
        for (int i=0; i<3; i++)
        {
            ui->signersTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
}

void KRReportForm::ownerDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.ownerId = ui->signersTable->item(curRow, 0)->text();
    ui->ownerEdit->setText(text);
}

void KRReportForm::member1Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member1Id = ui->signersTable->item(curRow, 0)->text();
    ui->member1Edit->setText(text);
}


void KRReportForm::member2Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member2Id = ui->signersTable->item(curRow, 0)->text();
    ui->member2Edit->setText(text);
}

void KRReportForm::member3Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member3Id = ui->signersTable->item(curRow, 0)->text();
    ui->member3Edit->setText(text);
}

void KRReportForm::repairerDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.repairerId = ui->signersTable->item(curRow, 0)->text();
    ui->repairerEdit->setText(text);
}

void KRReportForm::chiefDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.chiefId = ui->signersTable->item(curRow, 0)->text();
    ui->chiefEdit->setText(text);
}

void KRReportForm::doneButtonClicked()
{
    QString query, prepQuery;
    QStringList queryList;
    if (!checkFilling()) return;

    if (reportId == "0") {
        db->startTransaction();
        query = "INSERT INTO krreports (krr_desc, krr_unit, krr_planbeg, krr_planend, krr_docnum) "
                "VALUES ('%1', '%2', '%3', '%4', '%5')";
        query = query.arg(ui->descEdit->text());
        query = query.arg(ui->unitBox->currentData().toString());
        query = query.arg(ui->begDateEdit->text());
        query = query.arg(ui->endDateEdit->text());
        query = query.arg(ui->docNumEdit->text());

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
        reportId = db->lastInsertId().toString();
    } else {
        query = "UPDATE krreports SET krr_desc = '%1', krr_unit = '%2, krr_planbeg = '%3', krr_planend = '%4', krr_docnum = '%5' "
                "WHERE krr_id = '%6'";
        query = query.arg(ui->descEdit->text());
        query = query.arg(ui->unitBox->currentData().toString());
        query = query.arg(ui->begDateEdit->text());
        query = query.arg(ui->endDateEdit->text());
        query = query.arg(ui->docNumEdit->text());
        query = query.arg(reportId);

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }

        query = "DELETE FROM krrworks WHERE krw_work = '%1'";
        query = query.arg(reportId);
        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }

        query = "DELETE FROM krsigners WHERE krs_report = '%1'";
        query = query.arg(reportId);
        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
    }

    query = "INSERT INTO krrworks (krw_work, krw_report) VALUES ('%1', '%2')";

    for (int i=0; i<ui->selectedKRTable->rowCount(); i++)
    {
        prepQuery = query.arg(ui->selectedKRTable->item(i, 0)->text()).arg(reportId);
        if (!db->execQuery(prepQuery)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
    }

    query = "INSERT INTO krsigners (krs_report, krs_signer, krs_role) VALUES ('%1', '%2', '%3')";

    queryList.clear();
    prepQuery = query.arg(reportId).arg(signers.ownerId).arg(OWNER);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.member1Id).arg(MEMBER1);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.member2Id).arg(MEMBER2);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.member3Id).arg(MEMBER3);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.repairerId).arg(REPAIRER);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.chiefId).arg(CHIEF);
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

void KRReportForm::editReport(QString Id)
{
    int curRow;
    QString query;

    reportId = Id;

    query = "SELECT krr_desc, krr_unit, krr_planbeg, krr_planend, krr_docnum FROM krreports WHERE krr_id = '%1'";

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
        ui->docNumEdit->setText(db->fetchValue(4).toString());
    }

    query = "SELECT krw_work, sch_type, sch_kks, kr_begdate, kr_enddate FROM krrworks AS kw "
            "LEFT JOIN kaprepairs AS kr ON kr_id = krw_work "
            "LEFT JOIN schedule AS sch ON kr.kr_sched = sch.sch_id "
            "WHERE krw_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    ui->selectedKRTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        curRow = ui->selectedKRTable->rowCount();
        ui->selectedKRTable->insertRow(curRow);
        for (int i=0; i<5; i++)
        {
            ui->selectedKRTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->selectedKRTable->setSortingEnabled(true);

    query = "SELECT krs_signer, krs_role, sig_name, sig_loc FROM krsigners "
            "LEFT JOIN signers ON krs_signer = sig_id "
            "WHERE krs_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    while (db->nextRecord())
    {
        switch (db->fetchValue(1).toInt()) {
        case OWNER:
            signers.ownerId = db->fetchValue(0).toString();
            ui->ownerEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case MEMBER1:
            signers.member1Id = db->fetchValue(0).toString();
            ui->member1Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case MEMBER2:
            signers.member2Id = db->fetchValue(0).toString();
            ui->member2Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case MEMBER3:
            signers.member3Id = db->fetchValue(0).toString();
            ui->member3Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case REPAIRER:
            signers.repairerId = db->fetchValue(0).toString();
            ui->repairerEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case CHIEF:
            signers.chiefId = db->fetchValue(0).toString();
            ui->chiefEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        }
    }
}
