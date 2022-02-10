#include "movesform.h"
#include "ui_movesform.h"

MovesForm::MovesForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::MovesForm)
{
    ui->setupUi(this);

    connect(ui->materialBox, &QComboBox::currentTextChanged, this, &MovesForm::updateMoves);
}

MovesForm::~MovesForm()
{
    delete ui;
}

void MovesForm::setDatabase(Database *db)
{
    this->db = db;
}

void MovesForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateMaterials();
    updateMoves();
}

void MovesForm::updateMaterials()
{
    QString query = "SELECT DISTINCT rcm_material, mat_name FROM recmaterials "
                    "LEFT JOIN materials ON rcm_material = mat_id";
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    ui->materialBox->blockSignals(true);
    ui->materialBox->clear();
    while (db->nextRecord())
        ui->materialBox->addItem(db->fetchValue(1).toString(), db->fetchValue(0));

    query = "SELECT DISTINCT kam_material, mat_name FROM kradditionalmats "
            "LEFT JOIN materials ON kam_material = mat_id";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    while (db->nextRecord())
        if (ui->materialBox->findData(db->fetchValue(0)) < 0) {
            ui->materialBox->addItem(db->fetchValue(1).toString(), db->fetchValue(0));
        }
    ui->materialBox->blockSignals(false);
}

void MovesForm::updateMoves()
{
    int curRow;
    QString query;

    if (!ui->materialBox->count()) return;

    query = "SELECT rec_date, rcm_count, rec_description FROM recmaterials "
            "LEFT JOIN receipts ON rcm_rec = rec_id "
            "WHERE rcm_material = '%1'";
    query = query.arg(ui->materialBox->currentData().toString());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->incomesTable->rowCount()) ui->incomesTable->removeRow(0);
    while (db->nextRecord())
    {
        curRow = ui->incomesTable->rowCount();
        ui->incomesTable->insertRow(curRow);
        for (int i=0; i<3; i++)
        {
            ui->incomesTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->incomesTable->resizeColumnsToContents();

    query = "SELECT kr_enddate, kam_count, sch_name || ' ' || sch_type || ' ' || sch_kks AS dev FROM kradditionalmats "
            "LEFT JOIN kaprepairs ON kam_kr = kr_id "
            "LEFT JOIN schedule ON kr_sched = sch_id "
            "WHERE kam_material = '%1'";
    query = query.arg(ui->materialBox->currentData().toString());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->expensesTable->rowCount()) ui->expensesTable->removeRow(0);
    while (db->nextRecord())
    {
        curRow = ui->expensesTable->rowCount();
        ui->expensesTable->insertRow(curRow);
        for (int i=0; i<3; i++)
        {
            ui->expensesTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->expensesTable->resizeColumnsToContents();
}
