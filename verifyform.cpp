#include "verifyform.h"
#include "ui_verifyform.h"

VerifyForm::VerifyForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::VerifyForm)
{
    ui->setupUi(this);
    ui->table->hideColumn(0);
}

VerifyForm::~VerifyForm()
{
    delete ui;
}

void VerifyForm::setDatabase(Database *db)
{
    this->db = db;
}

void VerifyForm::showEvent(QShowEvent *event)
{
    int curRow;
    float income, expence;
    bool idFound;
    QString query;
    QWidget::showEvent(event);

    query = "SELECT rcm_material, mat_name, SUM(rcm_count) FROM recmaterials AS rcm "
            "LEFT JOIN materials AS m ON rcm.rcm_material = m.mat_id "
            "GROUP BY rcm_material, mat_name";
    if (!db->execQuery(query)) {
        db->showError(this);
        close();
        return;
    }

    while (ui->table->rowCount()) ui->table->removeRow(0);
    while (db->nextRecord())
    {
        curRow = ui->table->rowCount();
        ui->table->insertRow(curRow);
        for (int i=0; i<3; i++)
            ui->table->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        ui->table->setItem(curRow, 3, new QTableWidgetItem("0"));
    }

    query = "SELECT kam_material, mat_name, 'none', SUM(kam_count) FROM kradditionalmats AS kam "
            "LEFT JOIN materials AS m ON kam.kam_material = m.mat_id "
            "GROUP BY kam_material, mat_name";

    if (!db->execQuery(query)) {
        db->showError(this);
        close();
        return;
    }

    while (db->nextRecord())
    {
        curRow = ui->table->rowCount();
        idFound = false;
        for (int i=0; i<curRow; i++)
        {
            if (ui->table->item(i, 0)->text() == db->fetchValue(0)){
                idFound = true;
                ui->table->item(i, 3)->setText(db->fetchValue(3).toString());
                break;
            }
        }
        if (!idFound) {
            ui->table->insertRow(curRow);
            for (int i=0; i<4; i++)
            {
                if (i == 2) {
                    ui->table->setItem(curRow, i, new QTableWidgetItem("0"));
                    break;
                }
                ui->table->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
            }
        }
    }

    for (int i=0; i<ui->table->rowCount(); i++)
    {
        income = ui->table->item(i, 2)->text().toFloat();
        expence = ui->table->item(i, 3)->text().toFloat();
        ui->table->setItem(i, 4, new QTableWidgetItem(QString("%1").arg(income - expence)));
    }
}
