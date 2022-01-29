#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"
#include "dictionaryform.h"
#include "raspform.h"
#include "normativeform.h"
#include "defectform.h"

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
    NormativeForm *normativeForm;
    DefectForm *defectForm;
    QString makeRaspEquipments(QStringList equip);
    QString makeRaspWoktypes(QStringList wt);
private slots:
    void employeesTriggered();
    void scheduleTriggered();
    void unitsTriggered();
    void issuersTriggered();
    void locationsTriggered();
    void materialsTriggered();
    void normativeTriggered();
    void addRaspClicked();
    void raspFormClosed(RaspForm *sender);
    void updateRaspTable();
    void raspDateChanged();
    void editRaspClicked();
    void deleteRaspClicked();
    void selectAllRaspClicked();
    void printRaspClicked();
    void raspCellDoubleClicked(int row, int column);
    void editorInputAccepted(FieldEditor *editor);
    void editorInputRejected(FieldEditor *editor);
    void addDefectClicked();
    void updateDefectsTable();

protected:
    void showEvent(QShowEvent *event);

};
#endif // MAINWINDOW_H
