#include "receiptsform.h"
#include "ui_receiptsform.h"

ReceiptsForm::ReceiptsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiptsForm)
{
    ui->setupUi(this);

    ui->receiptsTable->hideColumn(0);

    connect(ui->updateButton, &QToolButton::clicked, this, &ReceiptsForm::updateTable);
    connect(ui->addButton, &QToolButton::clicked, this, &ReceiptsForm::addReceiptClicked);
    connect(ui->editButton, &QToolButton::clicked, this, &ReceiptsForm::editReceiptClicked);
    connect(ui->deleteButton, &QToolButton::clicked, this, &ReceiptsForm::deleteReceiptClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &ReceiptsForm::close);
}

ReceiptsForm::~ReceiptsForm()
{
    delete ui;
}

void ReceiptsForm::setDatabase(Database *db)
{
    this->db = db;
}

void ReceiptsForm::updateTable()
{
    int curRow;
    QString query = "SELECT rec_id, rec_date, rec_description FROM receipts";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->receiptsTable->rowCount()) ui->receiptsTable->removeRow(0);

    ui->receiptsTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        curRow = ui->receiptsTable->rowCount();
        ui->receiptsTable->insertRow(curRow);
        for (int i=0; i<3; i++)
            ui->receiptsTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
    }
    ui->receiptsTable->resizeColumnsToContents();
    ui->receiptsTable->setSortingEnabled(true);
}

void ReceiptsForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateTable();
}

void ReceiptsForm::addReceiptClicked()
{
    ReceiptForm *rf = new ReceiptForm();
    rf->setDatabase(db);
    rf->newReceipt();
    connect(rf, &ReceiptForm::receiptSaved, this, &ReceiptsForm::updateTable);
    connect(rf, &ReceiptForm::closed, this, &ReceiptsForm::receiptFormClosed);
    rf->show();
}

void ReceiptsForm::editReceiptClicked()
{
    if (!ui->receiptsTable->currentItem()) return;
    ReceiptForm *rf = new ReceiptForm();
    rf->setDatabase(db);
    rf->editReceipt(ui->receiptsTable->item(ui->receiptsTable->currentRow(), 0)->text());
    connect(rf, &ReceiptForm::receiptSaved, this, &ReceiptsForm::updateTable);
    connect(rf, &ReceiptForm::closed, this, &ReceiptsForm::receiptFormClosed);
    rf->show();
}

void ReceiptsForm::deleteReceiptClicked()
{
    if (!ui->receiptsTable->currentItem()) return;
    db->startTransaction();
    QString id = ui->receiptsTable->item(ui->receiptsTable->currentRow(), 0)->text();
    QString query = "DELETE FROM recmaterials WHERE rcm_rec ='%1'";
    query = query.arg(id);

    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return;
    }

    query = "DELETE FROM receipts WHERE rec_id = '%1'";
    query = query.arg(id);

    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return;
    }
    db->commitTransaction();
}

void ReceiptsForm::receiptFormClosed(ReceiptForm *sender)
{
    sender->deleteLater();
}
