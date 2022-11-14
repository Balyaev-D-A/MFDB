#include "krreportform.h"
#include "ui_krreportform.h"

#include <QMessageBox>

KRReportForm::KRReportForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::KRReportForm)
{
    QDate currDate = QDate::currentDate();
    ui->setupUi(this);
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setCurrentIndex(0);
    ui->krTable->setAcceptFrom(ui->selectedKRTable);
    ui->selectedKRTable->setAcceptFrom(ui->krTable);
    ui->selectedKRTable->setMovableRows(true);
    ui->krTable->hideColumn(0);
    ui->selectedKRTable->hideColumn(0);
    ui->selectedKRTable->setSortingEnabled(false);
    ui->begDateEdit->setDate(QDate(currDate.year(), currDate.month(), 1));
    ui->endDateEdit->setDate(QDate(currDate.year(), currDate.month(), currDate.daysInMonth()));
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
    ui->member2Edit->setStyleSheet("background: rgb(255, 255, 0);");
    ui->member3Edit->setStyleSheet("background: rgb(255, 255, 0);");
    ui->member4Edit->setStyleSheet("background: rgb(255, 255, 0);");
    ui->member5Edit->setStyleSheet("background: rgb(255, 255, 0);");
    connect(ui->unitBox, &QComboBox::currentTextChanged, this, &KRReportForm::unitChanged);
    connect(ui->addButton, &QToolButton::clicked, this, &KRReportForm::addButtonClicked);
    connect(ui->removeButton, &QToolButton::clicked, this, &KRReportForm::removeButtonClicked);
    connect(ui->krTable, &DragDropTable::itemDroped, this, &KRReportForm::removeButtonClicked);
    connect(ui->selectedKRTable, &DragDropTable::itemDroped, this, &KRReportForm::addButtonClicked);
    connect(ui->addAllButton, &QToolButton::clicked, this, &KRReportForm::addAllButtonClicked);
    connect(ui->removeAllButton, &QToolButton::clicked, this, &KRReportForm::removeAllButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &KRReportForm::close);
    connect(ui->nextButton, &QPushButton::clicked, this, &KRReportForm::nextButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &KRReportForm::backButtonClicked);
    connect(ui->ownerEdit, &DragDropEdit::itemDroped, this, &KRReportForm::ownerDroped);
    connect(ui->member1Edit, &DragDropEdit::itemDroped, this, &KRReportForm::member1Droped);
    connect(ui->member2Edit, &DragDropEdit::itemDroped, this, &KRReportForm::member2Droped);
    connect(ui->member3Edit, &DragDropEdit::itemDroped, this, &KRReportForm::member3Droped);
    connect(ui->member4Edit, &DragDropEdit::itemDroped, this, &KRReportForm::member4Droped);
    connect(ui->member5Edit, &DragDropEdit::itemDroped, this, &KRReportForm::member5Droped);
    connect(ui->repairerEdit, &DragDropEdit::itemDroped, this, &KRReportForm::repairerDroped);
    connect(ui->chiefEdit, &DragDropEdit::itemDroped, this, &KRReportForm::chiefDroped);
    connect(ui->doneButton, &QPushButton::clicked, this, &KRReportForm::doneButtonClicked);
    connect(ui->ownerClearButton, &QToolButton::clicked, this, &KRReportForm::ownerClearClicked);
    connect(ui->member1ClearButton, &QToolButton::clicked, this, &KRReportForm::member1ClearClicked);
    connect(ui->member2ClearButton, &QToolButton::clicked, this, &KRReportForm::member2ClearClicked);
    connect(ui->member3ClearButton, &QToolButton::clicked, this, &KRReportForm::member3ClearClicked);
    connect(ui->member4ClearButton, &QToolButton::clicked, this, &KRReportForm::member4ClearClicked);
    connect(ui->member5ClearButton, &QToolButton::clicked, this, &KRReportForm::member5ClearClicked);
    connect(ui->repairerClearButton, &QToolButton::clicked, this, &KRReportForm::repairerClearClicked);
    connect(ui->chiefClearButton, &QToolButton::clicked, this, &KRReportForm::chiefClearClicked);
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
    if (ui->docNumEdit->text() == "")
        ui->docNumEdit->setText(db->getVariable("НомерДокКР").toString());
}

void KRReportForm::updateKRTable()
{
    int curRow;
    QStringList usedIds;
    QStringList inReportIds;
    QString query = "SELECT krw_work, krw_report FROM krrworks";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord())
    {
        if (reportId != "0")
            if (db->fetchValue(1).toString() == reportId) continue;
        inReportIds.append(db->fetchValue(0).toString());
    }
    query = "SELECT kr_id, sch_type, sch_kks, kr_begdate, kr_enddate FROM kaprepairs AS kr "
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
        if (inReportIds.contains(db->fetchValue(0).toString())) continue;
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
    ui->selectedKRTable->resizeColumnsToContents();
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
    int curRow;
    for (int i=0; i<ui->krTable->rowCount(); i++)
    {
        curRow = ui->selectedKRTable->rowCount();
        ui->selectedKRTable->insertRow(curRow);
        for (int col=0; col<ui->selectedKRTable->columnCount(); col++)
        {
            ui->selectedKRTable->setItem(curRow, col, new QTableWidgetItem(ui->krTable->item(i, col)->text()));
        }
    }
    ui->selectedKRTable->resizeColumnsToContents();
    updateKRTable();
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

QString KRReportForm::maxDate(QString date1, QString date2)
{
    QStringList dateList1, dateList2;

    dateList1 = date1.split(".");
    dateList2 = date2.split(".");

    if (dateList1[2].toInt() > dateList1[2].toInt()) return date1;
    else if (dateList2[2].toInt() > dateList1[2].toInt()) return date2;
    else if (dateList1[1].toInt() > dateList2[1].toInt()) return date1;
    else if (dateList2[1].toInt() > dateList1[1].toInt()) return date2;
    else if (dateList1[0].toInt() > dateList2[0].toInt()) return date1;
    else if (dateList2[0].toInt() > dateList1[0].toInt()) return date2;
    return date1;
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
    QDate signDate = QDate::fromString(ui->dateEdit->text(), "dd.MM.yyyy");
    QString lastDate = "00.00.0000";
    for (int i=0; i<ui->selectedKRTable->rowCount(); i++)
    {
        lastDate = maxDate(lastDate, ui->selectedKRTable->item(i, 4)->text());
    }
    QDate lDate = QDate::fromString(lastDate, "dd.MM.yyyy");
    if (lDate.daysTo(signDate) < 0) {
        showErrorMessage("Дата подписания не может быть раньше даты окончания работ.");
        return false;
    }
    if (lDate.daysTo(signDate) > 7) {
        QMessageBox::StandardButton btn = QMessageBox::question(this, "Внимание!!!", "Между окончанием работ и датой подписания не должно быть больше 7 дней. Вы действительно хотите оставить даты как есть?");
        if (btn == QMessageBox::No) return false;
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
    QString sigId;
    QString query = "SELECT sig_id, sig_name, sig_loc FROM signers WHERE sig_hidden = 'FALSE'";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->signersTable->rowCount()) ui->signersTable->removeRow(0);

    while (db->nextRecord())
    {
        curRow = ui->signersTable->rowCount();

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

void KRReportForm::ownerDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.ownerId = ui->signersTable->item(curRow, 0)->text();
    ui->ownerEdit->setText(text);
    updateSignersTable();
}

void KRReportForm::member1Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member1Id = ui->signersTable->item(curRow, 0)->text();
    ui->member1Edit->setText(text);
    updateSignersTable();
}


void KRReportForm::member2Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member2Id = ui->signersTable->item(curRow, 0)->text();
    ui->member2Edit->setText(text);
    updateSignersTable();
}

void KRReportForm::member3Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member3Id = ui->signersTable->item(curRow, 0)->text();
    ui->member3Edit->setText(text);
    updateSignersTable();
}

void KRReportForm::member4Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member4Id = ui->signersTable->item(curRow, 0)->text();
    ui->member4Edit->setText(text);
    updateSignersTable();
}

void KRReportForm::member5Droped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.member5Id = ui->signersTable->item(curRow, 0)->text();
    ui->member5Edit->setText(text);
    updateSignersTable();
}

void KRReportForm::repairerDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.repairerId = ui->signersTable->item(curRow, 0)->text();
    ui->repairerEdit->setText(text);
    updateSignersTable();
}

void KRReportForm::chiefDroped()
{
    int curRow = ui->signersTable->currentRow();
    QString text = "%1 %2";
    text = text.arg(ui->signersTable->item(curRow, 1)->text()).arg(ui->signersTable->item(curRow, 2)->text());
    signers.chiefId = ui->signersTable->item(curRow, 0)->text();
    ui->chiefEdit->setText(text);
    updateSignersTable();
}

void KRReportForm::ownerClearClicked()
{
    ui->ownerEdit->clear();
    signers.ownerId = "";
    updateSignersTable();
}

void KRReportForm::member1ClearClicked()
{
    ui->member1Edit->clear();
    signers.member1Id = "";
    updateSignersTable();
}

void KRReportForm::member2ClearClicked()
{
    ui->member2Edit->clear();
    signers.member2Id = "";
    updateSignersTable();
}

void KRReportForm::member3ClearClicked()
{
    ui->member3Edit->clear();
    signers.member3Id = "";
    updateSignersTable();
}

void KRReportForm::member4ClearClicked()
{
    ui->member4Edit->clear();
    signers.member4Id = "";
    updateSignersTable();
}

void KRReportForm::member5ClearClicked()
{
    ui->member5Edit->clear();
    signers.member5Id = "";
    updateSignersTable();
}

void KRReportForm::repairerClearClicked()
{
    ui->repairerEdit->clear();
    signers.repairerId = "";
    updateSignersTable();
}

void KRReportForm::chiefClearClicked()
{
    ui->chiefEdit->clear();
    signers.chiefId = "";
    updateSignersTable();
}

void KRReportForm::doneButtonClicked()
{
    QString query, prepQuery;
    QStringList queryList;
    if (!checkFilling()) return;
    db->startTransaction();
    if (reportId == "0") {
        query = "INSERT INTO krreports (krr_desc, krr_unit, krr_planbeg, krr_planend, krr_date, krr_docnum) "
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
        query = "UPDATE krreports SET krr_desc = '%1', krr_unit = '%2', krr_planbeg = '%3', krr_planend = '%4', krr_date = '%5',  krr_docnum = '%6' "
                "WHERE krr_id = '%7'";
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

        query = "DELETE FROM krrworks WHERE krw_report = '%1'";
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

    query = "INSERT INTO krrworks (krw_work, krw_report, krw_order) VALUES ('%1', '%2', '%3')";

    for (int i=0; i<ui->selectedKRTable->rowCount(); i++)
    {
        prepQuery = query.arg(ui->selectedKRTable->item(i, 0)->text()).arg(reportId).arg(i);
        if (!db->execQuery(prepQuery)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
    }

    query = "INSERT INTO krsigners (krs_report, krs_signer, krs_role) VALUES ('%1', '%2', '%3')";

    queryList.clear();
    prepQuery = query.arg(reportId).arg(signers.ownerId).arg(KRSOWNER);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.member1Id).arg(KRSMEMBER1);
    queryList.append(prepQuery);
    if (signers.member2Id != "") {
        prepQuery = query.arg(reportId).arg(signers.member2Id).arg(KRSMEMBER2);
        queryList.append(prepQuery);
    }
    if (signers.member3Id != "") {
        prepQuery = query.arg(reportId).arg(signers.member3Id).arg(KRSMEMBER3);
        queryList.append(prepQuery);
    }
    if (signers.member4Id != "") {
        prepQuery = query.arg(reportId).arg(signers.member4Id).arg(KRSMEMBER4);
        queryList.append(prepQuery);
    }
    if (signers.member5Id != "") {
        prepQuery = query.arg(reportId).arg(signers.member5Id).arg(KRSMEMBER5);
        queryList.append(prepQuery);
    }
    prepQuery = query.arg(reportId).arg(signers.repairerId).arg(KRSREPAIRER);
    queryList.append(prepQuery);
    prepQuery = query.arg(reportId).arg(signers.chiefId).arg(KRSCHIEF);
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

    query = "SELECT krr_desc, krr_unit, krr_planbeg, krr_planend, krr_date, krr_docnum FROM krreports WHERE krr_id = '%1'";

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

    query = "SELECT krw_work, sch_type, sch_kks, kr_begdate, kr_enddate FROM krrworks AS kw "
            "LEFT JOIN kaprepairs AS kr ON kr_id = krw_work "
            "LEFT JOIN schedule AS sch ON kr.kr_sched = sch.sch_id "
            "WHERE krw_report = '%1' ORDER BY krw_order";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }


    while (db->nextRecord())
    {
        curRow = ui->selectedKRTable->rowCount();
        ui->selectedKRTable->insertRow(curRow);
        for (int i=0; i<5; i++)
        {
            ui->selectedKRTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->selectedKRTable->resizeColumnsToContents();

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
        case KRSOWNER:
            signers.ownerId = db->fetchValue(0).toString();
            ui->ownerEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case KRSMEMBER1:
            signers.member1Id = db->fetchValue(0).toString();
            ui->member1Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case KRSMEMBER2:
            signers.member2Id = db->fetchValue(0).toString();
            ui->member2Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case KRSMEMBER3:
            signers.member3Id = db->fetchValue(0).toString();
            ui->member3Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case KRSMEMBER4:
            signers.member4Id = db->fetchValue(0).toString();
            ui->member4Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case KRSMEMBER5:
            signers.member5Id = db->fetchValue(0).toString();
            ui->member5Edit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case KRSREPAIRER:
            signers.repairerId = db->fetchValue(0).toString();
            ui->repairerEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        case KRSCHIEF:
            signers.chiefId = db->fetchValue(0).toString();
            ui->chiefEdit->setText(db->fetchValue(2).toString() + " " + db->fetchValue(3).toString());
            break;
        }
    }
}
