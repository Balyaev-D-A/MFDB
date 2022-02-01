#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCalendarWidget>
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
    QCalendarWidget *datePicker;
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
    void raspEditorInputAccepted(FieldEditor *editor);
    void defEditorInputAccepted(FieldEditor *editor);
    void editorInputRejected(FieldEditor *editor);
    void addDefectClicked();
    void updateDefectsTable();
    void dtCellDoubleClicked(int row, int column);
    void datePicked(const QDate &date);
    void clearCellPressed();
    void editDefectClicked();
    void deleteDefectClicked();

protected:
    void showEvent(QShowEvent *event);

};
#endif // MAINWINDOW_H
