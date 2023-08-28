#include <QMessageBox>
#include "receiptform.h"
#include "ui_receiptform.h"

ReceiptForm::ReceiptForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ReceiptForm)
{
    ui->setupUi(this);

    ui->addedMatTable->hideColumn(0);
    ui->materialTable->hideColumn(0);
    ui->addedMatTable->setAcceptFrom(ui->materialTable);
    ui->materialTable->setAcceptFrom(ui->addedMatTable);

    connect(ui->addMaterialButton, &QToolButton::clicked, this, &ReceiptForm::addMaterialClicked);
    connect(ui->removeMaterialButton, &QToolButton::clicked, this, &ReceiptForm::removeMaterialClicked);
    connect(ui->addedMatTable, &DragDropTable::itemDroped, this, &ReceiptForm::addMaterialClicked);
    connect(ui->materialTable, &DragDropTable::itemDroped, this, &ReceiptForm::removeMaterialClicked);
    connect(ui->okButton, &QPushButton::clicked, this, &ReceiptForm::okClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ReceiptForm::cancelClicked);
}

ReceiptForm::~ReceiptForm()
{
    delete ui;
}

void ReceiptForm::setDatabase(Database *db)
{
    this->db = db;
}

void ReceiptForm::newReceipt()
{
    ui->dateEdit->setDate(QDate::currentDate());
    ui->descEdit->setText("");
    recId = "0";
    while (ui->addedMatTable->rowCount()) ui->addedMatTable->removeRow(0);
    updateMaterials();
    ui->okButton->setDisabled(false);
}

void ReceiptForm::editReceipt(QString recId)
{
    int curRow;
    QString query;
    QStringList dateList;
    this->recId = recId;
    matsChanged = false;
    ui->okButton->setDisabled(true);

    query = "SELECT rec_date, rec_description FROM receipts WHERE rec_id = '%1'";
    query = query.arg(recId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()) {
        dateList = db->fetchValue(0).toString().split(".");
        ui->dateEdit->setDate(QDate(dateList[2].toInt(), dateList[1].toInt(), dateList[0].toInt()));
        ui->descEdit->setText(db->fetchValue(1).toString());
    }
    else return;

    query = "SELECT rcm_material, mat_name, rcm_count FROM recmaterials AS rcm "
            "LEFT JOIN materials AS m ON rcm_material = m.mat_id "
            "WHERE rcm_rec = '%1'";
    query = query.arg(recId);

   if (!db->execQuery(query)) {
       db->showError(this);
       return;
   }
   while (ui->addedMatTable->rowCount()) ui->addedMatTable->removeRow(0);
   while (db->nextRecord())
   {
       curRow = ui->addedMatTable->rowCount();
       ui->addedMatTable->insertRow(curRow);
       for(int i=0; i<3; i++)
           ui->addedMatTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
   }
   ui->addedMatTable->resizeColumnsToContents();
   updateMaterials();
   ui->okButton->setDisabled(false);
}

void ReceiptForm::updateMaterials()
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

void ReceiptForm::addMaterialClicked()
{
    if (!ui->materialTable->currentItem()) return;
    ui->addedMatTable->setSortingEnabled(false);
    ui->addedMatTable->insertRow(ui->addedMatTable->rowCount());
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 0,
                               new QTableWidgetItem(ui->materialTable->item(ui->materialTable->currentRow(), 0)->text()));
    ui->addedMatTable->setItem(ui->addedMatTable->rowCount() - 1, 1,
                               new QTableWidgetItem(ui->materialTable->item(ui->materialTable->currentRow(), 1)->text()));
    ui->addedMatTable->setSortingEnabled(true);
    ui->materialTable->removeRow(ui->materialTable->currentRow());
    ui->addedMatTable->resizeColumnsToContents();
    matsChanged = true;
}

void ReceiptForm::removeMaterialClicked()
{
    ui->addedMatTable->removeRow(ui->addedMatTable->currentRow());
    matsChanged = true;
    updateMaterials();
}

bool ReceiptForm::saveReceipt()
{
    QString query;
    QString prepQuery;
    db->startTransaction();
    if (recId != "0") {
        if (matsChanged) {
            query = "DELETE FROM recmaterials WHERE rcm_rec = '%1'";
            query = query.arg(recId);
            if (!db->execQuery(query)) {
                db->showError(this);
                db->rollbackTransaction();
                return false;
            }

            query = "INSERT INTO recmaterials (rcm_rec, rcm_material, rcm_count) VALUES ('%1', '%2', '%3')";

            for (int i=0; i<ui->addedMatTable->rowCount(); i++)
            {
                prepQuery = query.arg(recId).arg(ui->addedMatTable->item(i, 0)->text()).arg(ui->addedMatTable->item(i, 2)->text());

                if (!db->execQuery(prepQuery)) {
                    db->showError(this);
                    db->rollbackTransaction();
                    return false;
                }
            }
        }

        query = "UPDATE receipts SET rec_date = '%1', rec_description = '%2' WHERE rec_id = '%3'";
        query = query.arg(ui->dateEdit->date().toString("dd.MM.yyyy")).arg(ui->descEdit->text()).arg(recId);

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }
    }
    else {
        query = "INSERT INTO receipts (rec_date, rec_description) VALUES ('%1', '%2')";
        query = query.arg(ui->dateEdit->date().toString("dd.MM.yyyy")).arg(ui->descEdit->text());

        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return false;
        }

        recId = db->lastInsertId().toString();

        query = "INSERT INTO recmaterials (rcm_rec, rcm_material, rcm_count) VALUES ('%1', '%2', '%3')";

        for (int i=0; i<ui->addedMatTable->rowCount(); i++)
        {
            prepQuery = query.arg(recId).arg(ui->addedMatTable->item(i, 0)->text()).arg(ui->addedMatTable->item(i, 2)->text());

            if (!db->execQuery(prepQuery)) {
                db->showError(this);
                db->rollbackTransaction();
                return false;
            }
        }
    }
    db->commitTransaction();
    emit receiptSaved();
    return true;
}

void ReceiptForm::cancelClicked()
{
    if (matsChanged){
        QMessageBox::StandardButton btn = QMessageBox::question(this, "Внимание!!!", "Вы действительно хотите закрыть окно без сохранения?");
        if (btn == QMessageBox::No) return;
    }
    close();
}

void ReceiptForm::okClicked()
{
    if (saveReceipt()) close();
}

void ReceiptForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed(this);
}
