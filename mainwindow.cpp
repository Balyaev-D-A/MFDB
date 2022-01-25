#include <QMessageBox>
#include <QShowEvent>
#include <QDebug>

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

    db = new Database;
    dictionaryForm = new DictionaryForm();
    dictionaryForm->setDatabase(db);
    normativeForm = new NormativeForm();
    normativeForm->setDatabase(db);
    defectForm = new DefectForm();
    defectForm->setDatabase(db);

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
        QMessageBox::critical(this, "Ошибка!!!", "Невозможно установить соединение с сервером БД!");
        return;
    }
    ui->raspDateEdit->setDate(QDate::currentDate());
    ui->taskDateEdit->setDate(QDate::currentDate());
    db->execQuery("select emp_name, emp_id from employees where emp_metrolog = false and emp_hidden = false order by emp_name");
    ui->employeeBox->addItem("Все", 0);
    while (db->nextRecord())
        ui->employeeBox->addItem(db->fetchValue(0).toString(), db->fetchValue(1));
    updateRaspTable();
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
    QString query = "select rasp_id, rasp_num, emp_name, unit_name, sum(sch_hours) from rasp r "
                    "left join requipment re on r.rasp_id = re.re_rasp "
                    "left join schedule sh on sh.sch_id = re.re_equip "
                    "left join units u on sh.sch_unit = u.unit_id "
                    "left join employees e on e.emp_id = r.rasp_executor "
                    "where rasp_date = '%1' group by emp_name, rasp_id, unit_name order by rasp_id";
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
    query = "select sch_type, re_worktype  from requipment re left join schedule sch on re.re_equip = sch.sch_id where re_rasp = ";
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
        connect(editor, &FieldEditor::acceptInput, this, &MainWindow::editorInputAccepted);
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

void MainWindow::editorInputAccepted(FieldEditor *editor)
{
    QString query = "update rasp set rasp_num = '%1' where rasp_id = '%2'";
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

    query = "delete from rasp where ";
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
    defectForm->show();
}
