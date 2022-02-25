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
    connect(ui->unitBox, &QComboBox::currentTextChanged, this, &KRReportForm::unitChanged);
    connect(ui->addButton, &QToolButton::clicked, this, &KRReportForm::addButtonClicked);
    connect(ui->removeButton, &QToolButton::clicked, this, &KRReportForm::removeButtonClicked);
    connect(ui->krTable, &DragDropTable::itemDroped, this, &KRReportForm::removeButtonClicked);
    connect(ui->selectedKRTable, &DragDropTable::itemDroped, this, &KRReportForm::addButtonClicked);
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
