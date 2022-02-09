#include <QDate>
#include <QMessageBox>
#include <QDebug>

#include "krselectorform.h"
#include "ui_krselectorform.h"

KRSelectorForm::KRSelectorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KRSelectorForm)
{
    ui->setupUi(this);
    ui->scheduleTable->hideColumn(0);
    connect(ui->cancelButton, &QPushButton::clicked, this, &KRSelectorForm::cancelClicked);
    connect(ui->okButton, &QPushButton::clicked, this, &KRSelectorForm::okClicked);
    connect(ui->scheduleTable, &QTableWidget::cellDoubleClicked, this, &KRSelectorForm::okClicked);
    connect(ui->monthBox, &QComboBox::currentTextChanged, this, &KRSelectorForm::updateTable);
}

KRSelectorForm::~KRSelectorForm()
{
    delete ui;
}

void KRSelectorForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateTable();
}

void KRSelectorForm::setDatabase(Database *db)
{
    this->db = db;
}

void KRSelectorForm::updateTable()
{
    QString query;
    QString date;
    QStringList usedKRs;
    int curRow;

    if (ui->monthBox->currentIndex() != 0) ui->scheduleTable->hideColumn(5);
    else ui->scheduleTable->showColumn(5);

    query = "SELECT kr_sched FROM kaprepairs";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord())
    {
        usedKRs.append(db->fetchValue(0).toString());
    }

    query = "SELECT sch_id, unit_name, sch_name, sch_type, sch_kks, sch_date from schedule AS s "
                    "LEFT JOIN units AS u ON s.sch_unit = unit_id where sch_worktype = 'КР' AND sch_executor = 'ИТЦРК'";
    if (ui->monthBox->currentIndex() != 0) {
        query += " AND sch_date = '%1'";
        if (ui->monthBox->currentIndex() < 10)
            date = QString("0%1.").arg(ui->monthBox->currentIndex());
        else if (ui->monthBox->currentIndex() < 13)
            date = QString("%1.").arg(ui->monthBox->currentIndex());
        else
            date = "ППР-";
        date += QDate::currentDate().toString("yyyy");
        query = query.arg(date);
    }

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->scheduleTable->rowCount()) ui->scheduleTable->removeRow(0);
    ui->scheduleTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        if (usedKRs.contains(db->fetchValue(0).toString())) continue;
        curRow = ui->scheduleTable->rowCount();
        ui->scheduleTable->insertRow(curRow);
        for (int i=0; i<6; i++)
        {
            ui->scheduleTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->scheduleTable->setSortingEnabled(true);
    ui->scheduleTable->resizeColumnsToContents();

}

void KRSelectorForm::cancelClicked()
{
    close();
}

void KRSelectorForm::okClicked()
{
    KRDevice result;

    if (!ui->scheduleTable->currentItem()) {
        QMessageBox::critical(this, "Ошибка!!!", "Не выбрано ни одного устройства");
        return;
    }

    result.sched = ui->scheduleTable->item(ui->scheduleTable->currentRow(), 0)->text();
    result.device = ui->scheduleTable->item(ui->scheduleTable->currentRow(), 2)->text();
    result.type = ui->scheduleTable->item(ui->scheduleTable->currentRow(), 3)->text();
    result.kks = ui->scheduleTable->item(ui->scheduleTable->currentRow(), 4)->text();

    emit selected(result);
    close();
}

void KRSelectorForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed(this);
}
