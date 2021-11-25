#include "raspform.h"
#include "ui_raspform.h"
#include <QMessageBox>
#include <QTextStream>

RaspForm::RaspForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RaspForm)
{
    ui->setupUi(this);
    connect(ui->newCheckBox, &QCheckBox::stateChanged, this, &RaspForm::updateWorkTable);
    connect(ui->inworkCheckBox, &QCheckBox::stateChanged, this, &RaspForm::updateWorkTable);
    connect(ui->completedCheckBox, &QCheckBox::stateChanged, this, &RaspForm::updateWorkTable);
    connect(ui->addMemberButton, &QToolButton::clicked, this, &RaspForm::addMemberClicked);
    connect(ui->removeMemberButton, &QToolButton::clicked, this, &RaspForm::removeMemberClicked);
    connect(ui->addWorkButton, &QToolButton::clicked, this, &RaspForm::addWorkClicked);
}

RaspForm::~RaspForm()
{
    delete ui;
}

void RaspForm::setDatabase(Database *db)
{
    this->db = db;
}

void RaspForm::newRasp()
{
    ui->dateEdit->setDate(QDate::currentDate());

    db->pq->exec("select iss_id, iss_name, iss_default from issuers");
    while (db->pq->next())
        ui->issuerBox->addItem(db->pq->value(1).toString(), db->pq->value(0));
    db->pq->seek(-1);
    while (db->pq->next())
        if (db->pq->value(2).toBool()) {
            ui->issuerBox->setCurrentText(db->pq->value(1).toString());
            break;
        }
    db->pq->exec("select unit_id, unit_name from units order by unit_name");
    while (db->pq->next())
        ui->unitBox->addItem(db->pq->value(1).toString(), db->pq->value(0));
    ui->unitBox->setCurrentIndex(0);
    ui->monthBox->setCurrentIndex(QDate::currentDate().month());

    ui->currWorkTable->hideColumn(0);
    ui->workTable->hideColumn(0);
    updateWorkTable();
    updateMembers();
    connect(ui->unitBox, &QComboBox::currentTextChanged, this, &RaspForm::updateWorkTable);
    connect(ui->monthBox, &QComboBox::currentTextChanged, this, &RaspForm::updateWorkTable);
}

void RaspForm::updateWorkTable()
{
    QString query;
    QString month = "";

    while (ui->workTable->rowCount()>0) ui->workTable->removeRow(0);

    query = "select sch_id, sch_kks, sch_type, sch_worktype, loc_location, sch_hours, sch_state from schedule left join locations on schedule.sch_kks = locations.loc_kks "
            "where schedule.sch_executor = 'ИТЦРК' and sch_unit = '" + ui->unitBox->currentData().toString() + "' and (sch_state = 16535";
    if (ui->newCheckBox->isChecked())
        query += " or sch_state = 0";
    if (ui->inworkCheckBox->isChecked())
        query += " or sch_state = 1";
    if (ui->completedCheckBox->isChecked())
        query += " or sch_state = 2";
    query += ")";
    if (ui->monthBox->currentIndex() != 0) {
        if (ui->monthBox->currentIndex() == 13){
            month = "ППР-" + QString::number(QDate::currentDate().year());
        } else {
            if (ui->monthBox->currentIndex() < 10) month = "0";
            month += QString::number(ui->monthBox->currentIndex()) + "." + QString::number(QDate::currentDate().year());
        }
    }
    if (month != "")
        query += "and sch_date = '" + month + "'";

    if (!db->pq->exec(query)) {
        QMessageBox::critical(this, "Ошибка выполнения запроса!", "Ошибка выполнения запроса: " + db->pq->lastError().text());
        return;
    }
    for (int i=0; db->pq->next(); i++)
    {
        ui->workTable->insertRow(i);
        for (int j=0; j<6; j++)
        {
            ui->workTable->setItem(i, j, new QTableWidgetItem(db->pq->value(j).toString()));
            switch (db->pq->value(6).toInt())
            {
            case 1:
                ui->workTable->item(i,j)->setBackground(QBrush(Qt::yellow));
                break;
            case 2:
                ui->workTable->item(i,j)->setBackground(QBrush(Qt::green));
                break;
            default:
                break;
            }
        }
    }
    updateTotal();
}

void RaspForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed(this);
}

void RaspForm::updateTotal()
{
    float total = 0;
    QString labelText;
    for (int i=0; i<ui->workTable->rowCount(); i++ )
        total += ui->workTable->item(i,5)->text().toFloat();
    labelText = QString("Итоговые трудозатраты: %1 часов.").arg(QString::number(total, 'f', 2));
    ui->labelTotal->setText(labelText);
}

void RaspForm::updateRaspTotal()
{
    float total = 0;
    QString labelText;
    for (int i=0; i<ui->currWorkTable->rowCount(); i++)
        total += ui->currWorkTable->item(i, 5)->text().toFloat();
    labelText = QString("Итоговые трудозатраты: %1 часов.").arg(QString::number(total, 'f', 2));
    ui->labelRaspTotal->setText(labelText);
}

void RaspForm::updateMembers()
{
    QListWidgetItem *it;
    EmpInfo info;
    QString query = "select emp_id, emp_name, emp_metrolog from employees where emp_hidden = false";
    if (!hasMA)
        query += " and emp_metrolog = false";
    if (!db->pq->exec(query)){
        db->showError(this);
        return;
    }
    empmap.clear();
    ui->teamList->clear();
    while (db->pq->next())
    {
        info.id = db->pq->value(0).toInt();
        info.metrolog = db->pq->value(2).toBool();
        empmap[db->pq->value(1).toString()] = info;
        it = new QListWidgetItem(db->pq->value(1).toString());
        if (info.metrolog)
            it->setIcon(QIcon(":/icons/icons/metrolog.png"));
        else
            it->setIcon(QIcon(":/icons/icons/technic.png"));
        ui->teamList->addItem(it);
    }
}

void RaspForm::addMemberClicked()
{
    QTreeWidgetItem *it;
    QTreeWidgetItem *child;
    if (!ui->teamList->currentItem()) return;
    if (ui->teamTree->topLevelItemCount() == 0) {
        if (empmap[ui->teamList->currentItem()->text()].metrolog){
            QMessageBox::warning(this, "Невозможно добавить производителя работ", "Метролог не может быть производителем работ.");
            return;
        }
        it = new QTreeWidgetItem();
        it->setText(0, ui->teamList->currentItem()->text());
        it->setIcon(0,ui->teamList->currentItem()->icon());
        ui->teamTree->addTopLevelItem(it);
        delete ui->teamList->takeItem(ui->teamList->currentRow());
    }
    else {
        it = ui->teamTree->topLevelItem(0);
        child = new QTreeWidgetItem();
        child->setText(0, ui->teamList->currentItem()->text());
        child->setIcon(0,ui->teamList->currentItem()->icon());
        it->addChild(child);
        it->setExpanded(true);
        delete ui->teamList->takeItem(ui->teamList->currentRow());
    }
}

void RaspForm::removeMemberClicked()
{
    QTreeWidgetItem *twItem;
    QTreeWidgetItem *parent;
    QListWidgetItem *lwItem;
    int index;

    twItem = ui->teamTree->currentItem();
    if (!twItem) return;
    parent = twItem->parent();
    if (parent) {
        index = parent->indexOfChild(twItem);
        twItem = parent->takeChild(index);
        lwItem = new QListWidgetItem();
        lwItem->setText(twItem->text(0));
        lwItem->setIcon(twItem->icon(0));
        ui->teamList->addItem(lwItem);
        delete twItem;
    }
    else {
        index = ui->teamTree->indexOfTopLevelItem(ui->teamTree->currentItem());
        parent = ui->teamTree->takeTopLevelItem(index);
        for (int i=0; i<parent->childCount(); i++)
        {
            twItem = parent->child(i);
            lwItem = new QListWidgetItem();
            lwItem->setText(twItem->text(0));
            lwItem->setIcon(twItem->icon(0));
            ui->teamList->addItem(lwItem);
        }
        lwItem = new QListWidgetItem();
        lwItem->setText(parent->text(0));
        lwItem->setIcon(parent->icon(0));
        ui->teamList->addItem(lwItem);
        delete parent;
    }
}

void RaspForm::addWorkClicked()
{
    QTableWidgetItem *it;
    int curRow;
    curRow = ui->workTable->currentRow();
    if (curRow < 0) return;
    ui->currWorkTable->insertRow(ui->currWorkTable->rowCount());
    for (int i=0; i<ui->workTable->columnCount(); i++)
    {
        it = new QTableWidgetItem();
        it->setText(ui->workTable->item(curRow, i)->text());
        ui->currWorkTable->setItem(ui->currWorkTable->rowCount(), i, it);
    }
}
