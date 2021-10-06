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
    db = new Database;
    dictionaryForm = new DictionaryForm();
    dictionaryForm->setDatabase(db);

    connect(ui->aEmployees, &QAction::triggered, this, &MainWindow::employeesTriggered);
    connect(ui->aSchedule, &QAction::triggered, this, &MainWindow::scheduleTriggered);
    connect(ui->aUnits, &QAction::triggered, this, &MainWindow::unitsTriggered);
    connect(ui->aLocations, &QAction::triggered, this, &MainWindow::locationsTriggered);
    connect(ui->addRaspButton, &QToolButton::clicked, this, &MainWindow::addRaspClicked);

}

MainWindow::~MainWindow()
{
    db->pdb->close();
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    if (!connectDB("127.0.0.1", "itcrk", "itcrk", "123321")) {
        QMessageBox::critical(this, "Ошибка", "Невозможно подключиться к базе");
    }
    ui->raspDateEdit->setDate(QDate::currentDate());
    ui->taskDateEdit->setDate(QDate::currentDate());
    db->pq->exec("select emp_name, emp_id from employees where emp_metrolog='false'");
    while (db->pq->next())
        ui->employeeBox->addItem(db->pq->value(0).toString(), db->pq->value(1));
    QMainWindow::showEvent(event);
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
    disconnect(sender, &RaspForm::closed, this, &MainWindow::raspFormClosed);
    delete sender;
}
