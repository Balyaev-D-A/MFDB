#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"
#include "dictionaryform.h"
#include "raspform.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool connectDB(QString host, QString dbname, QString user, QString password);

private:
    Ui::MainWindow *ui;
    Database *db;
    bool adminMode;
    DictionaryForm *dictionaryForm;
private slots:
    void employeesTriggered();
    void scheduleTriggered();
    void unitsTriggered();
    void issuersTriggered();
    void locationsTriggered();
    void addRaspClicked();
    void raspFormClosed(RaspForm *sender);

protected:
    void showEvent(QShowEvent *event);

};
#endif // MAINWINDOW_H
