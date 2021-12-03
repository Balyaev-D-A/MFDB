#include <QMessageBox>
#include <QShowEvent>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->raspTable->hideColumn(0);

    db = new Database;
    dictionaryForm = new DictionaryForm();
    dictionaryForm->setDatabase(db);

    connect(ui->aEmployees, &QAction::triggered, this, &MainWindow::employeesTriggered);
    connect(ui->aSchedule, &QAction::triggered, this, &MainWindow::scheduleTriggered);
    connect(ui->aUnits, &QAction::triggered, this, &MainWindow::unitsTriggered);
    connect(ui->aIssuers, &QAction::triggered, this, &MainWindow::issuersTriggered);
    connect(ui->aLocations, &QAction::triggered, this, &MainWindow::locationsTriggered);
    connect(ui->addRaspButton, &QToolButton::clicked, this, &MainWindow::addRaspClicked);
    connect(ui->raspDateEdit, &QDateEdit::dateChanged, this, &MainWindow::raspDateChanged);
}

MainWindow::~MainWindow()
{
    db->pdb->close();
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    if (!connectDB("127.0.0.1", "itcrk", "itcrk", "123321")) {
        db->showError(this);
    }
    ui->raspDateEdit->setDate(QDate::currentDate());
    ui->taskDateEdit->setDate(QDate::currentDate());
    db->pq->exec("select emp_name, emp_id from employees where emp_metrolog = false and emp_hidden = false order by emp_name");
    ui->employeeBox->addItem("Все", 0);
    while (db->pq->next())
        ui->employeeBox->addItem(db->pq->value(0).toString(), db->pq->value(1));
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
    if (!db->pq->exec(query)) {
        db->showError(this);
        return;
    }

    while (ui->raspTable->rowCount() > 0) ui->raspTable->removeRow(0);
    ui->raspTable->setSortingEnabled(false);
    for (int i=0; db->pq->next(); i++)
    {
        ui->raspTable->insertRow(i);
        ui->raspTable->setItem(i, 0, new QTableWidgetItem(db->pq->value(0).toString()));
        ui->raspTable->setItem(i, 1, new QTableWidgetItem(db->pq->value(1).toString()));
        ui->raspTable->setItem(i, 2, new QTableWidgetItem(db->pq->value(2).toString()));
        ui->raspTable->setItem(i, 3, new QTableWidgetItem(db->pq->value(3).toString()));
        ui->raspTable->setItem(i, 6, new QTableWidgetItem(db->pq->value(4).toString()));
    }
    query = "select sch_type, re_worktype  from requipment re left join schedule sch on re.re_equip = sch.sch_id where re_rasp = ";
    QStringList equip;
    QStringList wt;
    for (int i=0; i<ui->raspTable->rowCount(); i++)
    {
        if(!db->pq->exec(query + ui->raspTable->item(i, 0)->text())) {
            db->showError(this);
            return;
        }
        equip.clear();
        wt.clear();
        for (int j=0; db->pq->next(); j++)
        {
            equip.append(db->pq->value(0).toString());
            wt.append(db->pq->value(1).toString());
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
