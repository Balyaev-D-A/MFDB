#include <QMessageBox>
#include <QShowEvent>
#include <QDate>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fieldeditor.h"
#include "raspprintpreviewform.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->raspTable->hideColumn(0);
    ui->defectsTable->hideColumn(0);
    ui->krTable->hideColumn(0);

    int month = QDate::currentDate().month();
    int quarter;

    switch (month) {
    case 1:
    case 2:
    case 3:
        quarter = 1;
        break;
    case 4:
    case 5:
    case 6:
        quarter = 2;
        break;
    case 7:
    case 8:
    case 9:
        quarter = 3;
        break;
    case 10:
    case 11:
    case 12:
        quarter = 4;
        break;
    }

    ui->quarterBox->setCurrentIndex(quarter - 1);

    db = new Database;
    dictionaryForm = new DictionaryForm();
    dictionaryForm->setDatabase(db);
    normativeForm = new NormativeForm();
    normativeForm->setDatabase(db);
    defectForm = new DefectForm();
    defectForm->setDatabase(db);
    datePicker = new QCalendarWidget();
    datePicker->setWindowFlag(Qt::Popup, true);
    raspPicker = new QListWidget();
    raspPicker->setWindowFlag(Qt::Popup, true);
    krform = new KRForm();
    krform->setDatabase(db);
    receiptsForm = new ReceiptsForm();
    receiptsForm->setDatabase(db);

    connect(ui->aEmployees, &QAction::triggered, this, &MainWindow::employeesTriggered);
    connect(ui->aSchedule, &QAction::triggered, this, &MainWindow::scheduleTriggered);
    connect(ui->aUnits, &QAction::triggered, this, &MainWindow::unitsTriggered);
    connect(ui->aIssuers, &QAction::triggered, this, &MainWindow::issuersTriggered);
    connect(ui->aLocations, &QAction::triggered, this, &MainWindow::locationsTriggered);
    connect(ui->aMaterials, &QAction::triggered, this, &MainWindow::materialsTriggered);
    connect(ui->aNormative, &QAction::triggered, this, &MainWindow::normativeTriggered);
    connect(ui->addRaspButton, &QToolButton::clicked, this, &MainWindow::addRaspClicked);
    connect(ui->raspDateEdit, &QDateEdit::dateChanged, this, &MainWindow::raspDateChanged);
    connect(ui->editRaspButton, &QToolButton::clicked, this, &MainWindow::editRaspClicked);
    connect(ui->raspTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::raspCellDoubleClicked);
    connect(ui->deleteRaspButton, &QToolButton::clicked, this, &MainWindow::deleteRaspClicked);
    connect(ui->selectAllButton, &QToolButton::clicked, this, &MainWindow::selectAllRaspClicked);
    connect(ui->printButton, &QToolButton::clicked, this, &MainWindow::printRaspClicked);
    connect(ui->updateRaspButton, &QToolButton::clicked, this, &MainWindow::updateRaspTable);
    connect(ui->addDefectButton, &QToolButton::clicked, this, &MainWindow::addDefectClicked);
    connect(ui->updateDefectsButton, &QToolButton::clicked, this, &MainWindow::updateDefectsTable);
    connect(defectForm, &DefectForm::defectSaved, this, &MainWindow::updateDefectsTable);
    connect(ui->quarterBox, &QComboBox::currentTextChanged, this, &MainWindow::updateDefectsTable);
    connect(ui->defectsTable, &DefectsTable::cellDoubleClicked, this, &MainWindow::dtCellDoubleClicked);
    connect(datePicker, &QCalendarWidget::activated, this, &MainWindow::datePicked);
    connect(ui->defectsTable, &DefectsTable::clearCellPressed, this, &MainWindow::defClearCellPressed);
    connect(ui->editDefectButton, &QToolButton::clicked, this, &MainWindow::editDefectClicked);
    connect(ui->deleteDefectButton, &QToolButton::clicked, this, &MainWindow::deleteDefectClicked);
    connect(ui->addKRButton, &QToolButton::clicked, this, &MainWindow::addKRClicked);
    connect(ui->updateKRButton, &QToolButton::clicked, this, &MainWindow::updateKRTable);
    connect(krform, &KRForm::krSaved, this, &MainWindow::updateKRTable);
    connect(ui->krTable, &DefectsTable::cellDoubleClicked, this, &MainWindow::krCellDoubleClicked);
    connect(ui->krTable, &DefectsTable::clearCellPressed, this, &MainWindow::krClearCellPressed);
    connect(ui->editKRButton, &QToolButton::clicked, this, &MainWindow::editKRClicked);
    connect(ui->deleteKRButton, &QToolButton::clicked, this, &MainWindow::deleteKRClicked);
    connect(ui->krMonthBox, &QComboBox::currentTextChanged, this, &MainWindow::updateKRTable);
    connect(ui->receiptsAction, &QAction::triggered, this, &MainWindow::receiptsTriggered);
}

MainWindow::~MainWindow()
{
    db->close();
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    if (!connectDB("alpha.prostore.ru", "radico22", "radico", "coolpass")) {
        db->showError(this);
        return;
    }
    ui->raspDateEdit->setDate(QDate::currentDate());
    ui->taskDateEdit->setDate(QDate::currentDate());
    db->execQuery("SELECT emp_name, emp_id FROM employees WHERE emp_metrolog = false AND emp_hidden = false ORDER BY emp_name");
    ui->employeeBox->addItem("Все", 0);
    while (db->nextRecord())
        ui->employeeBox->addItem(db->fetchValue(0).toString(), db->fetchValue(1));
    updateRaspTable();
    updateDefectsTable();
    updateKRTable();
}

bool MainWindow::connectDB(QString host, QString dbname, QString user, QString password)
{
    if (!db->open(host, dbname, user, password)) return false;
    if (!db->deployTables()) return false;
    //TODO: нужно сделать авторизацию пользователя по ТЛД, если админов в базе нет, перейти в режим админа
    //db->pq->exec("select * from employees where emp_admin=true");
    adminMode = true;
    return true;
}

void MainWindow::employeesTriggered()
{
    dictionaryForm->setDictionary(EMPLOYEES);
    dictionaryForm->show();
}

void MainWindow::scheduleTriggered()
{
    dictionaryForm->setDictionary(SCHEDULE);
    dictionaryForm->show();
}

void MainWindow::unitsTriggered()
{
    dictionaryForm->setDictionary(UNITS);
    dictionaryForm->show();
}

void MainWindow::issuersTriggered()
{
    dictionaryForm->setDictionary(ISSUERS);
    dictionaryForm->show();
}

void MainWindow::locationsTriggered()
{
    dictionaryForm->setDictionary(LOCATIONS);
    dictionaryForm->show();
}

void MainWindow::materialsTriggered()
{
    dictionaryForm->setDictionary(MATERIALS);
    dictionaryForm->show();
}

void MainWindow::normativeTriggered()
{
    normativeForm->show();
}

void MainWindow::addRaspClicked()
{
    RaspForm *rf = new RaspForm();
    rf->setDatabase(db);
    connect(rf, &RaspForm::closed, this, &MainWindow::raspFormClosed);
    rf->newRasp();
    rf->show();
}

void MainWindow::raspFormClosed(RaspForm *sender)
{
    updateRaspTable();
    sender->deleteLater();
}

void MainWindow::updateRaspTable()
{
    QString query = "SELECT rasp_id, rasp_num, emp_name, unit_name, SUM(sch_hours) FROM rasp AS r "
                    "LEFT JOIN requipment AS re ON r.rasp_id = re.re_rasp "
                    "LEFT JOIN schedule AS sh ON sh.sch_id = re.re_equip "
                    "LEFT JOIN units AS u ON sh.sch_unit = u.unit_id "
                    "LEFT JOIN employees AS e ON e.emp_id = r.rasp_executor "
                    "WHERE rasp_date = '%1' GROUP BY emp_name, rasp_id, unit_name ORDER BY rasp_id";
    query = query.arg(ui->raspDateEdit->text());
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->raspTable->rowCount() > 0) ui->raspTable->removeRow(0);
    ui->raspTable->setSortingEnabled(false);
    for (int i=0; db->nextRecord(); i++)
    {
        ui->raspTable->insertRow(i);
        ui->raspTable->setItem(i, 0, new QTableWidgetItem(db->fetchValue(0).toString()));
        ui->raspTable->setItem(i, 1, new QTableWidgetItem(db->fetchValue(1).toString()));
        ui->raspTable->setItem(i, 2, new QTableWidgetItem(db->fetchValue(2).toString()));
        ui->raspTable->setItem(i, 3, new QTableWidgetItem(db->fetchValue(3).toString()));
        ui->raspTable->setItem(i, 6, new QTableWidgetItem(db->fetchValue(4).toString()));
    }
    query = "SELECT sch_type, re_worktype  FROM requipment AS re LEFT JOIN schedule AS sch ON re.re_equip = sch.sch_id WHERE re_rasp = ";
    QStringList equip;
    QStringList wt;
    for (int i=0; i<ui->raspTable->rowCount(); i++)
    {
        if(!db->execQuery(query + ui->raspTable->item(i, 0)->text())) {
            db->showError(this);
            return;
        }
        equip.clear();
        wt.clear();
        for (int j=0; db->nextRecord(); j++)
        {
            equip.append(db->fetchValue(0).toString());
            wt.append(db->fetchValue(1).toString());
        }
        ui->raspTable->setItem(i, 4, new QTableWidgetItem(makeRaspEquipments(equip)));
        ui->raspTable->setItem(i, 5, new QTableWidgetItem(makeRaspWoktypes(wt)));
    }
    ui->raspTable->setSortingEnabled(true);
    ui->raspTable->resizeColumnsToContents();
}

QString MainWindow::makeRaspEquipments(QStringList equip)
{
    QMap<QString, int> eqMap;
    QString result = "";

    for (int i=0; i<equip.size(); i++) eqMap[equip[i]] = 0;  //Заполняем ключи
    for (int i=0; i<equip.size(); i++) eqMap[equip[i]]++;    //Подсчет
    QMap<QString, int>::const_iterator i = eqMap.constBegin();
    while (i != eqMap.constEnd())
    {
        result += i.key() + " (" + QString("%1").arg(i.value()) + " шт.)";
        i++;
        if (i != eqMap.constEnd()) result += ", ";
    }
    return result;
}

QString MainWindow::makeRaspWoktypes(QStringList wt)
{
    QStringList wtList;
    QMap<QString, int> wtMap;
    QString result = "";

    for (int i=0; i<wt.size(); i++)
    {
        QStringList l = wt[i].split(",");
        wtList.append(l);
    }

    for (int i=0; i<wtList.size(); i++) wtMap[wtList[i]] = 0;
    for (int i=0; i<wtList.size(); i++) wtMap[wtList[i]]++;
    QMap<QString, int>::const_iterator i = wtMap.constBegin();
    while (i != wtMap.constEnd())
    {
        result += i.key().simplified() + "(" + QString("%1").arg(i.value()) + ")";
        i++;
        if (i != wtMap.constEnd()) result += ", ";
    }
    return result;
}

void MainWindow::raspDateChanged()
{
    updateRaspTable();
}

void MainWindow::editRaspClicked()
{
    if (ui->raspTable->currentRow() < 0) return;
    RaspForm *rf = new RaspForm();
    rf->setDatabase(db);
    connect(rf, &RaspForm::closed, this, &MainWindow::raspFormClosed);
    rf->editRasp(ui->raspTable->item(ui->raspTable->currentRow(), 0)->text());
    rf->show();
}

void MainWindow::raspCellDoubleClicked(int row, int column)
{
    if (column == 1) {
        FieldEditor *editor = new FieldEditor(ui->raspTable->viewport());
        editor->setType(ESTRING);
        editor->setInputMask("000/00");
        editor->setText(ui->raspTable->item(row, 1)->text());
        editor->setCell(row, 1);
        editor->setGeometry(ui->raspTable->visualItemRect(ui->raspTable->item(row, 1)));
        connect(editor, &FieldEditor::acceptInput, this, &MainWindow::raspEditorInputAccepted);
        connect(editor, &FieldEditor::rejectInput, this, &MainWindow::editorInputRejected);
        editor->show();
        editor->selectAll();
        editor->setFocus();
    }
    else {
        RaspForm *rf = new RaspForm();
        rf->setDatabase(db);
        connect(rf, &RaspForm::closed, this, &MainWindow::raspFormClosed);
        rf->editRasp(ui->raspTable->item(row, 0)->text());
        rf->show();
    }
}

void MainWindow::raspEditorInputAccepted(FieldEditor *editor)
{
    QString query = "UPDATE rasp SET rasp_num = '%1' WHERE rasp_id = '%2'";
    QStringList s = editor->text().split("/");
    QString num = s[0].simplified() + "/" + s[1].simplified();
    query = query.arg(num).arg(ui->raspTable->item(editor->getRow(), 0)->text());
    if (db->execQuery(query)) {
        updateRaspTable();
    }
    else {
        db->showError(this);
    }
    editor->hide();
    editor->deleteLater();
}

void MainWindow::editorInputRejected(FieldEditor *editor)
{
    editor->hide();
    editor->deleteLater();
}

void MainWindow::deleteRaspClicked()
{
    QList<QTableWidgetSelectionRange> selRanges;
    QStringList raspsToDel;
    QString query;

    selRanges = ui->raspTable->selectedRanges();
    foreach (QTableWidgetSelectionRange r, selRanges)
    {
        for (int i=r.topRow(); i<=r.bottomRow(); i++)
        {
            raspsToDel.append(ui->raspTable->item(i, 0)->text());
        }
    }

    query = "DELETE FROM rasp WHERE ";
    for (int i=0; i<raspsToDel.count(); i++)
    {
        query += "rasp_id = '" + raspsToDel[i] + "'";
        if (i < raspsToDel.count()-1) query += " or ";
    }

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    for (int i=ui->raspTable->rowCount()-1; i>=0; i--)
    {
        if (raspsToDel.contains(ui->raspTable->item(i, 0)->text())) ui->raspTable->removeRow(i);
    }
}

void MainWindow::selectAllRaspClicked()
{
    ui->raspTable->selectAll();
}

void MainWindow::printRaspClicked()
{
    QStringList raspList;
    RaspPrintPreviewForm *rpp;
    QList<QTableWidgetSelectionRange> selRanges;

    selRanges = ui->raspTable->selectedRanges();
    if (selRanges.isEmpty()) return;
    foreach (QTableWidgetSelectionRange s, selRanges)
    {
        for (int i=s.topRow(); i<=s.bottomRow(); i++)
        {
            raspList.append(ui->raspTable->item(i, 0)->text());
        }
    }
    rpp = new RaspPrintPreviewForm();
    rpp->setDatabase(db);
    rpp->showPreview(raspList);
    return;
}

void MainWindow::addDefectClicked()
{
    defectForm->newDefect();
    defectForm->show();
}

void MainWindow::updateDefectsTable()
{
    int rc;
    QString query = "SELECT def_id, def_num, unit_name, def_devtype, def_kks, def_begdate, def_enddate, def_rasp FROM defects AS d "
                    "LEFT JOIN schedule AS s ON d.def_kks = s.sch_kks "
                    "LEFT JOIN units AS u ON s.sch_unit = u.unit_id "
                    "WHERE def_quarter = '%1'";
    query = query.arg(ui->quarterBox->currentIndex() + 1);
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->defectsTable->rowCount()) ui->defectsTable->removeRow(0);
    ui->defectsTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        rc = ui->defectsTable->rowCount();
        ui->defectsTable->insertRow(rc);
        for (int i=0; i<8; i++)
        {
                ui->defectsTable->setItem(rc, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->defectsTable->setSortingEnabled(true);
    ui->defectsTable->sortItems(0);
    ui->defectsTable->resizeColumnsToContents();
}

void MainWindow::dtCellDoubleClicked(int row, int column)
{
    QRect itemRect;
    QPoint calOrigin;
    QDate minDate, maxDate;

    if (column == 1) {
        FieldEditor *fe = new FieldEditor(ui->defectsTable->viewport());
        fe->setGeometry(ui->defectsTable->visualItemRect(ui->defectsTable->item(row, column)));
        fe->setType(EINT);
        fe->setCell(row, column);
        connect(fe, &FieldEditor::acceptInput, this, &MainWindow::defEditorInputAccepted);
        connect(fe, &FieldEditor::rejectInput, this, &MainWindow::editorInputRejected);
        fe->setText(ui->defectsTable->item(row, column)->text());
        fe->show();
        fe->selectAll();
        fe->setFocus();
        return;
    }
    if (column == 7) {
        FieldEditor *fe = new FieldEditor(ui->defectsTable->viewport());
        fe->setGeometry(ui->defectsTable->visualItemRect(ui->defectsTable->item(row, column)));
        fe->setType(ESTRING);
        fe->setInputMask("000/00");
        fe->setCell(row, column);
        connect(fe, &FieldEditor::acceptInput, this, &MainWindow::defEditorInputAccepted);
        connect(fe, &FieldEditor::rejectInput, this, &MainWindow::editorInputRejected);
        fe->setText(ui->defectsTable->item(row, column)->text());
        fe->show();
        fe->selectAll();
        fe->setFocus();
        return;
    }

    if (column == 5 || column == 6) {
        switch (ui->quarterBox->currentIndex() + 1) {
        case 1:
            minDate.setDate(QDate::currentDate().year(), 1, 1);
            maxDate.setDate(QDate::currentDate().year(), 3, 31);
            break;
        case 2:
            minDate.setDate(QDate::currentDate().year(), 4, 1);
            maxDate.setDate(QDate::currentDate().year(), 6, 30);
            break;
        case 3:
            minDate.setDate(QDate::currentDate().year(), 7, 1);
            maxDate.setDate(QDate::currentDate().year(), 9, 30);
            break;
        case 4:
            minDate.setDate(QDate::currentDate().year(), 10, 1);
            maxDate.setDate(QDate::currentDate().year(), 12, 31);
            break;
        }
        defectChangingDate = true;
        int leftFrameWidth = this->geometry().left() - this->pos().x();
        int topFrameHeight = this->geometry().top() - this->pos().y();
        itemRect = ui->defectsTable->visualItemRect(ui->defectsTable->item(row, column));
        calOrigin = ui->defectsTable->mapToGlobal(itemRect.bottomLeft());
        datePicker->move(calOrigin.x() + leftFrameWidth, calOrigin.y() + topFrameHeight);
        datePicker->setDateRange(minDate, maxDate);
        if (QDate::currentDate() < minDate)
            datePicker->setSelectedDate(minDate);
        else if (QDate::currentDate() > maxDate)
            datePicker->setSelectedDate(maxDate);
        else
            datePicker->setSelectedDate(QDate::currentDate());

        datePicker->show();
        return;
    }
    editDefectClicked();
}

void MainWindow::datePicked(const QDate &date)
{
    QString query;
    int curRow, curCol;
    if (defectChangingDate) {
        defectChangingDate = false;
        query = "UPDATE defects SET %1 = '%2' WHERE def_id = '%3'";
        curRow = ui->defectsTable->currentItem()->row();
        curCol = ui->defectsTable->currentItem()->column();
        if (curCol == 5)
            query = query.arg("def_begdate");
        else if (curCol == 6)
            query = query.arg("def_enddate");
        else {
            datePicker->close();
            return;
        }
        query = query.arg(date.toString("dd.MM.yyyy"));
        query = query.arg(ui->defectsTable->item(curRow, 0)->text());
        if (!db->execQuery(query)) {
            db->showError(this);
            datePicker->close();
            return;
        }
        ui->defectsTable->currentItem()->setText(date.toString("dd.MM.yyyy"));
        datePicker->close();
    }
    else if (krChangingDate) {
        krChangingDate = false;
        query = "UPDATE kaprepairs SET %1 = '%2' WHERE kr_id = '%3'";
        curRow = ui->krTable->currentRow();
        curCol = ui->krTable->currentColumn();
        if (curCol == 5)
            query = query.arg("kr_begdate");
        else if (curCol == 6)
            query = query.arg("kr_enddate");
        else {
            datePicker->close();
            return;
        }
        query = query.arg(date.toString("dd.MM.yyyy"));
        query = query.arg(ui->krTable->item(curRow, 0)->text());

        if (!db->execQuery(query)) {
            db->showError(this);
            datePicker->close();
            return;
        }

        ui->krTable->currentItem()->setText(date.toString("dd.MM.yyyy"));
        datePicker->close();
    }
}

void MainWindow::defClearCellPressed()
{
    int curRow, curCol;
    if (!ui->defectsTable->currentItem()) return;
    curRow = ui->defectsTable->currentItem()->row();
    curCol = ui->defectsTable->currentItem()->column();
    if (curCol != 5)
        if (curCol != 6)
            if (curCol != 7)
                return;
    QMessageBox::StandardButton btn;
    btn = QMessageBox::question(this, "Внимание!!!", "Вы действительно хотите очистить значение?");
    if (btn == QMessageBox::No) return;
    QString query = "UPDATE defects SET %1 = NULL WHERE def_id = '%2'";
    if (curCol == 5)
        query = query.arg("def_begdate");
    else if (curCol == 6)
        query = query.arg("def_enddate");
    else if (curCol == 7)
        query = query.arg("def_rasp");
    query = query.arg(ui->defectsTable->item(curRow, 0)->text());
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    ui->defectsTable->currentItem()->setText("");
}

void MainWindow::defEditorInputAccepted(FieldEditor *editor)
{
    QString query = "UPDATE defects SET %1 = %2 WHERE def_id = '%3'";
    if (editor->getColumn() == 1)
        query = query.arg("def_num");
    else if (editor->getColumn() == 7)
        query = query.arg("def_rasp");
    else {
        editor->close();
        editor->deleteLater();
        return;
    }
    if (editor->text().simplified() == "" || editor->text().simplified() == "/")
        query = query.arg("NULL");
    else
        query = query.arg("'"+editor->text()+"'");
    query = query.arg(ui->defectsTable->item(editor->getRow(), 0)->text());
    if (!db->execQuery(query)) {
        db->showError(this);
        editor->close();
        editor->deleteLater();
        return;
    }
    if (editor->text().simplified() == "/")
        ui->defectsTable->item(editor->getRow(), editor->getColumn())->setText("");
    else
        ui->defectsTable->item(editor->getRow(), editor->getColumn())->setText(editor->text());
    editor->close();
    editor->deleteLater();
}

void MainWindow::editDefectClicked()
{
    if (!ui->defectsTable->currentItem()) return;

    defectForm->editDefect(ui->defectsTable->item(ui->defectsTable->currentRow(), 0)->text());
    defectForm->show();
}

void MainWindow::deleteDefectClicked()
{
    QString query;

    if (!ui->defectsTable->currentItem()) return;

    db->startTransaction();
    query = "DELETE FROM defadditionalmats WHERE dam_defect = '%1'";
    query = query.arg(ui->defectsTable->item(ui->defectsTable->currentRow(), 0)->text());
    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return;
    }

    query = "DELETE FROM defects WHERE def_id = '%1'";
    query = query.arg(ui->defectsTable->item(ui->defectsTable->currentRow(), 0)->text());
    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return;
    }

    ui->defectsTable->removeRow(ui->defectsTable->currentRow());
    db->commitTransaction();
}

void MainWindow::addKRClicked()
{
    krform->newKR();
    krform->show();
}

void MainWindow::updateKRTable()
{
    int curRow;
    QString month = "";
    QString query = "SELECT kr_id, unit_name, sch_name, sch_type, sch_kks, kr_begdate, kr_enddate FROM kaprepairs AS kr "
                    "LEFT JOIN schedule AS sch ON kr.kr_sched = sch.sch_id "
                    "LEFT JOIN units AS u ON sch.sch_unit = u.unit_id";
    if (ui->krMonthBox->currentIndex() != 0) {
        query += " WHERE sch.sch_date = '%1'";
        if (ui->krMonthBox->currentIndex() < 10)
            month = QString("0%1.").arg(ui->krMonthBox->currentIndex());
        else if (ui->krMonthBox->currentIndex() < 13)
            month = QString("%1.").arg(ui->krMonthBox->currentIndex());
        else
            month = "ППР-";
        month += QDate::currentDate().toString("yyyy");
        query = query.arg(month);
    }
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (ui->krTable->rowCount()) ui->krTable->removeRow(0);
    ui->krTable->setSortingEnabled(false);
    while (db->nextRecord())
    {
        curRow = ui->krTable->rowCount();
        ui->krTable->insertRow(curRow);
        for (int i=0; i<7; i++)
        {
            ui->krTable->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->krTable->setSortingEnabled(true);
    ui->krTable->resizeColumnsToContents();
}

void MainWindow::krCellDoubleClicked(int row, int column)
{
    QRect itemRect;
    QPoint calOrigin;
    QDate minDate, maxDate;

    if (column == 5 || column == 6) {
        krChangingDate = true;
        int leftFrameWidth = this->geometry().left() - this->pos().x();
        int topFrameHeight = this->geometry().top() - this->pos().y();
        itemRect = ui->krTable->visualItemRect(ui->krTable->item(row, column));
        calOrigin = ui->krTable->mapToGlobal(itemRect.bottomLeft());
        datePicker->move(calOrigin.x() + leftFrameWidth, calOrigin.y() + topFrameHeight);
        minDate.setDate(QDate::currentDate().year(), 1, 1);
        maxDate.setDate(QDate::currentDate().year(),12, 31);
        datePicker->setDateRange(minDate, maxDate);
        datePicker->setSelectedDate(QDate::currentDate());
        datePicker->show();
    }
    else
        editKRClicked();
}

void MainWindow::krClearCellPressed()
{
    int curRow, curCol;
    if (!ui->krTable->currentItem()) return;
    curRow = ui->krTable->currentRow();
    curCol = ui->krTable->currentColumn();
    if (curCol != 5)
        if (curCol != 6)
            return;
    QMessageBox::StandardButton btn;
    btn = QMessageBox::question(this, "Внимание!!!", "Вы действительно хотите очистить значение?");
    if (btn == QMessageBox::No) return;
    QString query = "UPDATE kaprepairs SET %1 = NULL WHERE kr_id = '%2'";
    if (curCol == 5)
        query = query.arg("kr_begdate");
    else if (curCol == 6)
        query = query.arg("kr_enddate");
    query = query.arg(ui->krTable->item(curRow, 0)->text());
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    ui->krTable->currentItem()->setText("");
}

void MainWindow::editKRClicked()
{
    if (!ui->krTable->currentItem()) return;
    krform->editKR(ui->krTable->item(ui->krTable->currentRow(), 0)->text());
    krform->show();
}

void MainWindow::deleteKRClicked()
{
    QString query;
    QString krId;

    if (!ui->krTable->currentItem()) return;

    krId = ui->krTable->item(ui->krTable->currentRow(), 0)->text();

    db->startTransaction();
    query = "DELETE FROM kradditionalmats WHERE kam_kr = '%1'";
    query = query.arg(krId);

    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return;
    }

    query = "DELETE FROM kaprepairs WHERE kr_id = '%1'";
    query = query.arg(krId);

    if (!db->execQuery(query)) {
        db->showError(this);
        db->rollbackTransaction();
        return;
    }

    ui->krTable->removeRow(ui->krTable->currentRow());
    db->commitTransaction();
}

void MainWindow::receiptsTriggered()
{
    receiptsForm->show();
}
