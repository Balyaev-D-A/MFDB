#include "materialsreportform.h"
#include "ui_materialsreportform.h"

MaterialsReportForm::MaterialsReportForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::MaterialsReportForm)
{
    ui->setupUi(this);

    connect(ui->materialBox, &QComboBox::currentTextChanged, this, &MaterialsReportForm::updateMaterials);
    connect(ui->workBox, &QComboBox::currentTextChanged, this, &MaterialsReportForm::updateMaterials);
    connect(ui->onlyDoneBox, &QCheckBox::stateChanged, this, &MaterialsReportForm::updateMaterials);
}

MaterialsReportForm::~MaterialsReportForm()
{
    delete ui;
}

void MaterialsReportForm::setDatabase(Database *db)
{
    this->db = db;
}

void MaterialsReportForm::updateTRMaterials()
{
    QList<QStringList> defectsTab;
    QStringList defectsStr;
    float amount, total;
    int curRow;

    if (!ui->materialBox->count()) return;

    while (ui->materialTable->rowCount()) ui->materialTable->removeRow(0);

    QString query ="SELECT def_id, def_devtype, def_kks, def_enddate FROM defects";

    if (ui->onlyDoneBox->isChecked()) {
        query +=" WHERE def_enddate IS NOT NULL";
    }

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord())
    {
        defectsStr.clear();
        for (int i=0; i<4; i++)
        {
            defectsStr.append(db->fetchValue(i).toString());
        }
        defectsTab.append(defectsStr);
    }

    total = 0;
    ui->materialTable->setSortingEnabled(false);
    for (int i=0; i<defectsTab.count(); i++)
    {
       amount = 0;
       query = "SELECT nm_count FROM normativmat WHERE nm_material = '%1' AND nm_dev = '%2' AND nm_worktype = '%3'";
       query = query.arg(ui->materialBox->currentData().toString()).arg(defectsTab[i][1]).arg("ТР");

       if (!db->execQuery(query)) {
           db->showError(this);
           return;
       }

       if (db->nextRecord()) {
           amount = db->fetchValue(0).toFloat();
       }

       query = "SELECT dam_count FROM defadditionalmats WHERE dam_material = '%1' AND dam_defect = '%2'";
       query = query.arg(ui->materialBox->currentData().toString()).arg(defectsTab[i][0]);

       if (!db->execQuery(query)) {
           db->showError(this);
           return;
       }

       if (db->nextRecord()) {
           amount = db->fetchValue(0).toFloat();
       }

       if (amount != 0) {
           total +=amount;
           curRow = ui->materialTable->rowCount();
           ui->materialTable->insertRow(curRow);
           ui->materialTable->setItem(curRow, 0, new QTableWidgetItem(QString("%1").arg(amount)));
           ui->materialTable->setItem(curRow, 1, new QTableWidgetItem(defectsTab[i][1] + " " + defectsTab[i][2]));
           ui->materialTable->setItem(curRow, 2, new QTableWidgetItem(defectsTab[i][3]));
       }
    }
    ui->materialTable->setSortingEnabled(true);
    curRow = ui->materialTable->rowCount();
    ui->materialTable->insertRow(curRow);
    ui->materialTable->setItem(curRow, 0, new QTableWidgetItem(QString("Итого: %1").arg(total)));
    QFont font = ui->materialTable->item(curRow, 0)->font();
    font.setBold(true);
    ui->materialTable->item(curRow, 0)->setFont(font);
    ui->materialTable->resizeColumnsToContents();
}

void MaterialsReportForm::updateKRMaterials()
{
    QList<QStringList> krTab;
    QStringList krStr;
    float amount, total;
    int curRow;

    if (!ui->materialBox->count()) return;

    while (ui->materialTable->rowCount()) ui->materialTable->removeRow(0);

    QString query ="SELECT kr_id, sch_type, sch_kks, kr_enddate FROM kaprepairs "
                   "LEFT JOIN schedule ON sch_id = kr_sched";

    if (ui->onlyDoneBox->isChecked()) {
        query +=" WHERE kr_enddate IS NOT NULL";
    }

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord())
    {
        krStr.clear();
        for (int i=0; i<3; i++)
        {
            krStr.append(db->fetchValue(i).toString());
        }
        krTab.append(krStr);
    }

    total = 0;
    ui->materialTable->setSortingEnabled(false);
    for (int i=0; i<krTab.count(); i++)
    {
       amount = 0;
       query = "SELECT nm_count FROM normativmat WHERE nm_material = '%1' AND nm_dev = '%2' AND nm_worktype = '%3'";
       query = query.arg(ui->materialBox->currentData().toString()).arg(krTab[i][1]).arg("КР");

       if (!db->execQuery(query)) {
           db->showError(this);
           return;
       }

       if (db->nextRecord()) {
           amount = db->fetchValue(0).toFloat();
       }

       query = "SELECT kam_count FROM kradditionalmats WHERE kam_material = '%1' AND kam_kr = '%2'";
       query = query.arg(ui->materialBox->currentData().toString()).arg(krTab[i][0]);

       if (!db->execQuery(query)) {
           db->showError(this);
           return;
       }

       if (db->nextRecord()) {
           amount = db->fetchValue(0).toFloat();
       }

       if (amount != 0) {
           total +=amount;
           curRow = ui->materialTable->rowCount();
           ui->materialTable->insertRow(curRow);
           ui->materialTable->setItem(curRow, 0, new QTableWidgetItem(QString("%1").arg(amount)));
           ui->materialTable->setItem(curRow, 1, new QTableWidgetItem(krTab[i][1] + " " + krTab[i][2]));
           ui->materialTable->setItem(curRow, 2, new QTableWidgetItem(krTab[i][3]));
       }
    }
    ui->materialTable->setSortingEnabled(true);
    curRow = ui->materialTable->rowCount();
    ui->materialTable->insertRow(curRow);
    ui->materialTable->setItem(curRow, 0, new QTableWidgetItem(QString("Итого: %1").arg(total)));
    QFont font = ui->materialTable->item(curRow, 0)->font();
    font.setBold(true);
    ui->materialTable->item(curRow, 0)->setFont(font);
    ui->materialTable->resizeColumnsToContents();
}

void MaterialsReportForm::updateMaterialBox()
{
    ui->materialBox->blockSignals(true);
    ui->materialBox->clear();
    QString query = "SELECT mat_id, mat_name FROM materials ORDER BY mat_name";
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    while (db->nextRecord())
    {
         ui->materialBox->addItem(db->fetchValue(1).toString(), db->fetchValue(0));
    }
    ui->materialBox->blockSignals(false);
}

void MaterialsReportForm::showEvent(QShowEvent *event)
{
    updateMaterialBox();
    updateMaterials();
    QWidget::showEvent(event);
}

void MaterialsReportForm::updateMaterials()
{
    if (ui->workBox->currentText() == "ТР") {
        updateTRMaterials();
    } else {
        updateKRMaterials();
    }
}

void MaterialsReportForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed();
}
