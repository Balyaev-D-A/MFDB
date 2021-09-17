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
    df = new DictionaryForm();
    df->setDatabase(db);

    connect(ui->aEmployees, &QAction::triggered, this, &MainWindow::employeesTriggered);
    connect(ui->aSchedule, &QAction::triggered, this, &MainWindow::scheduleTriggered);
    connect(ui->aUnits, &QAction::triggered, this, &MainWindow::unitsTriggered);

}

MainWindow::~MainWindow()
{
    db->pdb->close();
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    event->accept();
    if (!connectDB("127.0.0.1", "itcrk", "itcrk", "123321")) {
        QMessageBox::critical(this, "Ошибка", "Невозможно подключиться к базе");
    }
}

bool MainWindow::connectDB(QString host, QString dbname, QString user, QString password)
{
    if (!db->open(host, dbname, user, password)) return false;
    qDebug() << db->pdb->isValid();
//    if (!db->deployTables()) return false;
    //TODO: нужно сделать авторизацию пользователя по ТЛД, если админов в базе нет, перейти в режим админа
    //db->pq->exec("select * from employees where emp_admin=true");
    adminMode = true;
    return true;
}

void MainWindow::employeesTriggered()
{
    df->setDictionary(EMPLOYEES);
    df->show();
}

void MainWindow::scheduleTriggered()
{
    df->setDictionary(SCHEDULE);
    df->show();
}

void MainWindow::unitsTriggered()
{
    df->setDictionary(UNITS);
    df->show();
}
