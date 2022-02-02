#include "raspform.h"
#include "ui_raspform.h"
#include <QMessageBox>
#include <QTextStream>


RaspForm::RaspForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RaspForm)
{
    ui->setupUi(this);
    wEditor = new FieldEditor(ui->workTable->viewport());
    wEditor->hide();
    cwEditor = new FieldEditor(ui->currWorkTable->viewport());
    cwEditor->hide();
    wEditor->setType(ESTRING);
    cwEditor->setType(ESTRING);
    wtWidget = new WorkTypeWidget(ui->currWorkTable->viewport());
    wtWidget->hide();
    ui->currWorkTable->setAcceptFrom(ui->workTable);
    ui->workTable->setAcceptFrom(ui->currWorkTable);
    ui->teamList->setAcceptFrom(ui->teamTree);
    ui->teamTree->setAcceptFrom(ui->teamList);
    connect(ui->newCheckBox, &QCheckBox::stateChanged, this, &RaspForm::updateWorkTable);
    connect(ui->inworkCheckBox, &QCheckBox::stateChanged, this, &RaspForm::updateWorkTable);
    connect(ui->completedCheckBox, &QCheckBox::stateChanged, this, &RaspForm::updateWorkTable);
    connect(ui->addMemberButton, &QToolButton::clicked, this, &RaspForm::addMemberClicked);
    connect(ui->removeMemberButton, &QToolButton::clicked, this, &RaspForm::removeMemberClicked);
    connect(ui->addWorkButton, &QToolButton::clicked, this, &RaspForm::addWorkClicked);
    connect(ui->removeWorkButton, &QToolButton::clicked, this, &RaspForm::removeWorkClicked);
    connect(ui->unitBox, &QComboBox::currentTextChanged, this, &RaspForm::unitChanged);
    connect(ui->monthBox, &QComboBox::currentTextChanged, this, &RaspForm::updateWorkTable);
    connect(ui->workTable, &QTableWidget::cellDoubleClicked, this, &RaspForm::wCellDblClicked);
    connect(ui->currWorkTable, &QTableWidget::cellDoubleClicked, this, &RaspForm::cwCellDblClicked);
    connect(wEditor, &FieldEditor::acceptInput, this, &RaspForm::wInputAccepted);
    connect(wEditor, &FieldEditor::rejectInput, this, &RaspForm::wInputRejected);
    connect(cwEditor, &FieldEditor::acceptInput, this, &RaspForm::cwInputAccepted);
    connect(cwEditor, &FieldEditor::rejectInput, this, &RaspForm::cwInputRejected);
    connect(ui->currWorkTable, &DragDropTable::itemDroped, this, &RaspForm::addWorkClicked);
    connect(ui->workTable, &DragDropTable::itemDroped, this, &RaspForm::removeWorkClicked);
    connect(ui->teamList, &DragDropList::itemDroped, this, &RaspForm::removeMemberClicked);
    connect(ui->teamTree, &DragDropTree::itemDroped, this, &RaspForm::addMemberClicked);
    connect(wtWidget, &WorkTypeWidget::typeChanged, this, &RaspForm::workTypesChanged);
    connect(ui->saveButton, &QPushButton::clicked, this, &RaspForm::saveButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &RaspForm::cancelButtonClicked);
}

RaspForm::~RaspForm()
{
    wEditor->deleteLater();
    cwEditor->deleteLater();
    wtWidget->deleteLater();
    delete ui;
}

void RaspForm::setDatabase(Database *db)
{
    this->db = db;
}

void RaspForm::newRasp()
{
    ui->unitBox->blockSignals(true);
    ui->monthBox->blockSignals(true);
    ui->dateEdit->setDate(QDate::currentDate());

    db->execQuery("select iss_id, iss_name, iss_default from issuers");
    while (db->nextRecord())
        ui->issuerBox->addItem(db->fetchValue(1).toString(), db->fetchValue(0));
    db->seekRecord(-1);
    while (db->nextRecord())
        if (db->fetchValue(2).toBool()) {
            ui->issuerBox->setCurrentText(db->fetchValue(1).toString());
            break;
        }
    db->execQuery("select unit_id, unit_name from units order by unit_name");
    while (db->nextRecord())
        ui->unitBox->addItem(db->fetchValue(1).toString(), db->fetchValue(0));
    lastUnitIndex = 0;
    ui->unitBox->setCurrentIndex(0);
    ui->monthBox->setCurrentIndex(QDate::currentDate().month());
    ui->unitBox->blockSignals(false);
    ui->monthBox->blockSignals(false);
    ui->currWorkTable->hideColumn(0);
    ui->workTable->hideColumn(0);
    updateWorkTable();
    updateMembers();
    teamChanged = false;
    worksChanged = false;
}

void RaspForm::updateWorkTable()
{
    QString query;
    QString month = "";
    QStringList choosedWorks;
    QStringList idsInRasps;
    int row;
    bool done;
    bool inRasps;
    bool colored;
    Qt::GlobalColor color;

    query = "SELECT re_equip FROM requipment";
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    idsInRasps.clear();

    while (db->nextRecord())
    {
        idsInRasps.append(db->fetchValue(0).toString());
    }

    while (ui->workTable->rowCount()>0) ui->workTable->removeRow(0);

    query = "select sch_id, sch_kks, sch_type, sch_worktype, loc_location, sch_hours, sch_done from schedule left join locations on schedule.sch_kks = locations.loc_kks "
            "where schedule.sch_executor = 'ИТЦРК' and sch_unit = '%1'";
    query = query.arg(ui->unitBox->currentData().toString());
    if (ui->monthBox->currentIndex() != 0) {
        if (ui->monthBox->currentIndex() == 13){
            month = "ППР-" + QString::number(QDate::currentDate().year());
        } else {
            if (ui->monthBox->currentIndex() < 10) month = "0";
            month += QString::number(ui->monthBox->currentIndex()) + "." + QString::number(QDate::currentDate().year());
        }
    }
    if (month != "")
        query += " and sch_date = '" + month + "'";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    choosedWorks.clear();
    for (int i=0; i<ui->currWorkTable->rowCount(); i++)
    {
        choosedWorks.append(ui->currWorkTable->item(i, 0)->text());
    }

    ui->workTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        if (choosedWorks.contains(db->fetchValue(0).toString())) continue;
        done = db->fetchValue(6).toBool();
        inRasps = idsInRasps.contains(db->fetchValue("sch_id").toString());
        if (!ui->completedCheckBox->isChecked() && done) continue;
        if (!ui->inworkCheckBox->isChecked() && inRasps) continue;
        if (!ui->newCheckBox->isChecked() && !done && !inRasps) continue;
        if (done) {
            colored = true;
            color = Qt::green;
        }
        else if (inRasps) {
            colored = true;
            color = Qt::yellow;
        }
        else
            colored = false;

        row = ui->workTable->rowCount();
        ui->workTable->insertRow(row);
        for (int j=0; j<6; j++)
        {
            ui->workTable->setItem(row, j, new QTableWidgetItem(db->fetchValue(j).toString()));
            if (colored)
                ui->workTable->item(row, j)->setBackground(QBrush(color));
        }
    }
    ui->workTable->setSortingEnabled(true);
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
    if (!db->execQuery(query)){
        db->showError(this);
        return;
    }
    empmap.clear();
    ui->teamList->clear();
    while (db->nextRecord())
    {
        info.id = db->fetchValue(0).toInt();
        info.metrolog = db->fetchValue(2).toBool();
        empmap[db->fetchValue(1).toString()] = info;
        if (memberAdded(db->fetchValue(1).toString())) continue;
        it = new QListWidgetItem(db->fetchValue(1).toString());
        if (info.metrolog)
            it->setIcon(QIcon(":/icons/metrolog.png"));
        else
            it->setIcon(QIcon(":/icons/technic.png"));
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
    teamChanged = true;
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
    teamChanged = true;
}

void RaspForm::addWorkClicked()
{
    QTableWidgetItem *it;
    int curRow;
    curRow = ui->workTable->currentRow();
    if (curRow < 0) return;
    ui->currWorkTable->setSortingEnabled(false);
    ui->currWorkTable->insertRow(ui->currWorkTable->rowCount());
    for (int i=0; i<ui->workTable->columnCount(); i++)
    {
        it = new QTableWidgetItem();
        it->setText(ui->workTable->item(curRow, i)->text());
        ui->currWorkTable->setItem(ui->currWorkTable->rowCount()-1, i, it);
    }
    ui->workTable->removeRow(curRow);
    ui->currWorkTable->setSortingEnabled(true);
    updateRaspTotal();
    worksChanged = true;
}

void RaspForm::removeWorkClicked()
{
    if (ui->currWorkTable->currentRow() < 0) return;
    ui->currWorkTable->removeRow(ui->currWorkTable->currentRow());
    updateWorkTable();
    updateRaspTotal();
    worksChanged = true;
}

void RaspForm::unitChanged(const QString &text)
{
    QMessageBox *mb;
    QMessageBox::StandardButton btn;

    if (ui->currWorkTable->rowCount() > 0) {
        mb = new QMessageBox();
        btn = mb->question(this, "Смена блока", "В распоряжение нельзя добавлять работы с разных блоков. "
                                                   "При смене блока список работ будет очищен. Вы действительно хотите сменить блок?");
        delete mb;
        if (btn == QMessageBox::Yes) {
            while (ui->currWorkTable->rowCount()>0)
                ui->currWorkTable->removeRow(0);
            lastUnitIndex = ui->unitBox->findText(text);
        }
        else {
        ui->unitBox->blockSignals(true);
        ui->unitBox->setCurrentIndex(lastUnitIndex);
        ui->unitBox->blockSignals(false);
        }
    }
    lastUnitIndex = ui->unitBox->currentIndex();
    updateWorkTable();
}

void RaspForm::wCellDblClicked(int row, int column)
{
    if (column != 4) return;
    wEditor->setCell(row, column);
    wEditor->setGeometry(ui->workTable->visualItemRect(ui->workTable->item(row, column)));
    wEditor->setText(ui->workTable->item(row, column)->text());
    wEditor->show();
    wEditor->setFocus();
}

void RaspForm::cwCellDblClicked(int row, int column)
{
    if (column == 4) {
        cwEditor->setCell(row, column);
        cwEditor->setGeometry(ui->currWorkTable->visualItemRect(ui->currWorkTable->item(row, column)));
        cwEditor->setText(ui->currWorkTable->item(row, column)->text());
        cwEditor->show();
        cwEditor->setFocus();
    }
    if (column == 3) {
        QRect r = ui->currWorkTable->visualItemRect(ui->currWorkTable->item(row, column));
        wtWidget->setGeometry(r.x()+r.width(), r.y(), wtWidget->width(), wtWidget->height());
        wtWidget->setWorkTypes(ui->currWorkTable->item(row, column)->text());
        wtWidget->show();
    }
}

void RaspForm::wInputAccepted()
{
    QString kks = ui->workTable->item(wEditor->getRow(), 1)->text();
    QString query = "select loc_kks from locations where loc_kks = '" + kks + "'";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    if (db->affectedRows() > 0) {
        query = "update locations set loc_location = '" + wEditor->text() + "' where loc_kks = '" + kks + "'";
        if (!db->execQuery(query)) {
            db->showError(this);
            return;
        }
    }
    else {
        query = "insert into locations (loc_kks, loc_location) values ('" + kks + "', '" + wEditor->text() + "')";
        if (!db->execQuery(query)) {
            db->showError(this);
            return;
        }
    }
    ui->workTable->item(wEditor->getRow(), wEditor->getColumn())->setText(wEditor->text());
    wEditor->hide();
}

void RaspForm::cwInputAccepted()
{
    QString kks = ui->currWorkTable->item(cwEditor->getRow(), 1)->text();
    QString query = "select loc_kks from locations where loc_kks = '" + kks + "'";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    if (db->affectedRows() > 0) {
        query = "update locations set loc_location = '" + cwEditor->text() + "' where loc_kks = '" + kks + "'";
        if (!db->execQuery(query)) {
            db->showError(this);
            return;
        }
    }
    else {
        query = "insert into locations (loc_kks, loc_location) values ('" + kks + "', '" + cwEditor->text() + "')";
        if (!db->execQuery(query)) {
            db->showError(this);
            return;
        }
    }
    ui->currWorkTable->item(cwEditor->getRow(), cwEditor->getColumn())->setText(cwEditor->text());
    cwEditor->hide();
}

void RaspForm::wInputRejected()
{
    wEditor->hide();
}

void RaspForm::cwInputRejected()
{
    cwEditor->hide();
}

void RaspForm::saveButtonClicked()
{
    if (ui->currWorkTable->rowCount() == 0) {
        QMessageBox::critical(this, "Невозможно сохранить распоряжение!", "Список работ по распоряжению пуст. Добавьте работы по распоряжению.");
        return;
    }

    if (ui->teamTree->topLevelItemCount() == 0) {
        QMessageBox::critical(this, "Невозможно сохранить распоряжение!", "Нет ни одного человека в бригаде. Добавьте сотрудников в бригаду.");
        return;
    }

    QString query;

    QStringList s = ui->numEdit->text().split("/");
    QString num = s[0].simplified() + "/" + s[1].simplified();

    if (currentRasp < 0) {
        query = "insert into rasp (rasp_num, rasp_date, rasp_issuer, rasp_completed, rasp_executor) "
                "values ('%1', '%2', '%3', '%4', "
                "(select emp_id from employees where emp_name = '%5' limit 1))";

        query = query.arg(num);
        query = query.arg(ui->dateEdit->text());
        query = query.arg(ui->issuerBox->currentData().toInt());
        if (ui->allCompletedCheckBox->isChecked())
            query = query.arg("true");
        else
            query = query.arg("false");
        query = query.arg(ui->teamTree->topLevelItem(0)->text(0));

        db->startTransaction();
        if (!db->execQuery(query)) {
            QMessageBox::information(this, "QUERY", query);
            db->rollbackTransaction();
            db->showError(this);
            return;
        }

        int id = db->lastInsertId().toInt();
        query = "insert into rmembers (rm_rasp, rm_emp) values (%1, "
                "(select emp_id from employees where emp_name = '%2' limit 1))";
        QTreeWidgetItem *it = ui->teamTree->topLevelItem(0);

        for (int i=0; i<it->childCount(); i++)
        {
            if (!db->execQuery(query.arg(id).arg(it->child(i)->text(0)))) {
                db->rollbackTransaction();
                db->showError(this);
                return;
            }
        }

        query = "insert into requipment (re_rasp, re_equip, re_worktype) values (%1, %2, '%3')";
        QString q;
        for (int i=0; i<ui->currWorkTable->rowCount(); i++)
        {
            q = query.arg(id);
            q = q.arg(ui->currWorkTable->item(i, 0)->text());
            q = q.arg(ui->currWorkTable->item(i, 3)->text());
            if (!db->execQuery(q)) {
                db->rollbackTransaction();
                db->showError(this);
                return;
            }
        }
        db->commitTransaction();
    }
    else {
        db->startTransaction();
        query = "update rasp set rasp_num = '%1', rasp_date = '%2', "
                "rasp_issuer = %3, rasp_completed = %4, "
                "rasp_executor = (select emp_id from employees where emp_name = '%5' limit 1)";

        query = query.arg(num);
        query = query.arg(ui->dateEdit->text());
        query = query.arg(ui->issuerBox->currentData().toInt());
        if (ui->allCompletedCheckBox->isChecked())
            query = query.arg("true");
        else
            query = query.arg("false");
        query = query.arg(ui->teamTree->topLevelItem(0)->text(0));
        if (!db->execQuery(query)) {
            db->rollbackTransaction();
            db->showError(this);
            return;
        }
        if (teamChanged) {
            query = "delete from rmembers where rm_rasp = " + QString("%1").arg(currentRasp);
            if (!db->execQuery(query)) {
                db->rollbackTransaction();
                db->showError(this);
                return;
            }
            query = "insert into rmembers (rm_rasp, rm_emp) values (%1, "
                    "(select emp_id from employees where emp_name = '%2' limit 1))";
            QTreeWidgetItem *it = ui->teamTree->topLevelItem(0);

            for (int i=0; i<it->childCount(); i++)
            {
                if (!db->execQuery(query.arg(currentRasp).arg(it->child(i)->text(0)))) {
                    db->rollbackTransaction();
                    db->showError(this);
                    return;
                }
            }
        }

        if (worksChanged) {
            query = "delete from requipment where re_rasp = " + QString("%1").arg(currentRasp);
            if (!db->execQuery(query)) {
                db->rollbackTransaction();
                db->showError(this);
                return;
            }

            query = "insert into requipment (re_rasp, re_equip, re_worktype) values (%1, %2, '%3')";
            QString q;
            for (int i=0; i<ui->currWorkTable->rowCount(); i++)
            {
                q = query.arg(currentRasp);
                q = q.arg(ui->currWorkTable->item(i, 0)->text());
                q = q.arg(ui->currWorkTable->item(i, 3)->text());
                if (!db->execQuery(q)) {
                    db->rollbackTransaction();
                    db->showError(this);
                    return;
                }
            }
        }
        db->commitTransaction();
    }
    close();
}

void RaspForm::checkMA()
{
    hasMA = false;
    for (int i=0; i<ui->currWorkTable->rowCount(); i++)
    {
        if (ui->currWorkTable->item(i, 3)->text().contains("МА"))
            hasMA = true;
    }
}

void RaspForm::workTypesChanged()
{
    ui->currWorkTable->item(ui->currWorkTable->currentRow(), 3)->setText(wtWidget->workTypes());
    checkMA();
    checkAddedMembers();
    updateMembers();
    worksChanged = true;
}

void RaspForm::checkAddedMembers()
{
    QTreeWidgetItem *it = ui->teamTree->topLevelItem(0);
    QTreeWidgetItem *child;
    if (it) {
        for (int i=0; i<it->childCount(); i++)
        {
            child = it->child(i);
            if (empmap[child->text(0)].metrolog)
                if (!hasMA) {
                    delete it->takeChild(i);
                }
        }
    }
}

bool RaspForm::memberAdded(QString member)
{
    QTreeWidgetItem *it = ui->teamTree->topLevelItem(0);

    if (!it) return false;
    if (it->text(0) == member) return true;
    for (int i=0; i<it->childCount(); i++)
    {
        if (it->child(i)->text(0) == member) return true;
    }
    return false;
}

void RaspForm::cancelButtonClicked()
{
    if ((ui->currWorkTable->rowCount() > 0) |
        (ui->teamTree->topLevelItemCount() > 0))
    {
        QMessageBox::StandardButton btn = QMessageBox::question(this, "Внимание!!!", "Все изменения будут уничтожены. Вы действительно хотите отменить изменения?");
        if (btn == QMessageBox::No)
            return;
    }
    close();
}

bool RaspForm::editRasp(QString raspId)
{
    int executorId;
    QString query;
    int unitId;

    currentRasp = raspId.toInt();

    query = "select sch_id, sch_kks, sch_type, re_worktype, loc_location, sch_hours, sch_unit from requipment re "
            "left join schedule sch on sch.sch_id = re_equip "
            "left join locations loc on loc.loc_kks = sch.sch_kks "
            "where re_rasp = " + raspId;
    if (!db->execQuery(query)) {
       db->showError(this);
       return false;
    }

    ui->currWorkTable->setSortingEnabled(false);
    for (int i=0; db->nextRecord(); i++)
    {
        ui->currWorkTable->insertRow(i);
        for (int j=0; j<6; j++)
        {
            ui->currWorkTable->setItem(i, j, new QTableWidgetItem(db->fetchValue(j).toString()));
        }
    }
    ui->currWorkTable->setSortingEnabled(true);
    db->firstRecord();
    unitId = db->fetchValue(6).toInt();

    ui->unitBox->blockSignals(true);
    ui->monthBox->blockSignals(true);

    db->execQuery("select iss_id, iss_name, iss_default from issuers");
    while (db->nextRecord())
        ui->issuerBox->addItem(db->fetchValue(1).toString(), db->fetchValue(0));

    db->execQuery("select unit_id, unit_name from units order by unit_name");
    while (db->nextRecord())
        ui->unitBox->addItem(db->fetchValue(1).toString(), db->fetchValue(0));
    ui->unitBox->setCurrentIndex(ui->unitBox->findData(unitId));
    lastUnitIndex = ui->unitBox->currentIndex();
    ui->unitBox->blockSignals(false);
    ui->monthBox->blockSignals(false);
    ui->currWorkTable->hideColumn(0);
    ui->workTable->hideColumn(0);

    checkMA();
    updateWorkTable();
    updateMembers();

    query = "select rasp_num, rasp_date, rasp_issuer, rasp_executor, rasp_completed from rasp "
                    "where rasp_id = " + raspId;
    if (!db->execQuery(query)) {
        db->showError(this);
        return false;
    }

    if (db->nextRecord())
    {
        QString num = db->fetchValue(0).toString();
        QString date = db->fetchValue(1).toString();
        QVariant issuer = db->fetchValue(2);
        bool completed = db->fetchValue(4).toBool();
        executorId = db->fetchValue(3).toInt();
        ui->numEdit->setText(num);
        QStringList d = date.split(".");
        ui->dateEdit->setDate(QDate(d[2].toInt(), d[1].toInt(), d[0].toInt()));
        ui->monthBox->setCurrentIndex(d[1].toInt());
        ui->issuerBox->setCurrentIndex(ui->issuerBox->findData(issuer));
        ui->completedCheckBox->setChecked(completed);
    }

    for (int i=0; i<ui->teamList->count(); i++)
    {
        if (empmap[ui->teamList->item(i)->text()].id == executorId) {
            ui->teamList->setCurrentRow(i);
            addMemberClicked();
            break;
        }
    }

    query = "select rm_emp from rmembers where rm_rasp = " + raspId;
    if (!db->execQuery(query)) {
        db->showError(this);
        return false;
    }

    while (db->nextRecord())
    {
        for (int i=0; i<ui->teamList->count(); i++)
        {
            if (empmap[ui->teamList->item(i)->text()].id == db->fetchValue(0).toInt()) {
                ui->teamList->setCurrentRow(i);
                addMemberClicked();
                break;
            }
        }
    }
    teamChanged = false;
    worksChanged = false;
    return true;
}
