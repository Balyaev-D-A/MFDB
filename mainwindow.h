#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QListWidget>
#include "database.h"
#include "dictionaryform.h"
#include "raspform.h"
#include "normativeform.h"
#include "defectform.h"
#include "krform.h"
#include "receiptsform.h"
#include "verifyform.h"
#include "movesform.h"
#include "krreportsform.h"
#include "trreportsform.h"
#include "materialsreportform.h"
#include "settings.h"
#include "settingsform.h"

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
    bool defectChangingDate = false;
    bool krChangingDate = false;
    DictionaryForm *dictionaryForm;
    NormativeForm *normativeForm;
    DefectForm *defectForm;
    QCalendarWidget *datePicker;
    QListWidget *raspPicker;
    KRForm *krform;
    ReceiptsForm *receiptsForm;
    VerifyForm *verifyForm;
    MovesForm *movesForm;
    KRReportsForm *krReportsForm;
    TRReportsForm *trReportsForm;
    MaterialsReportForm *mrForm;
    SettingsForm *settingsForm;
    Settings *settings;
    QString makeRaspEquipments(QStringList equip);
    QString makeRaspWoktypes(QStringList wt);
    QString groupWorks(QList<QStringList> workList);
    void saveRasp(QStringList raspList);
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
    void defClearCellPressed();
    void editDefectClicked();
    void deleteDefectClicked();
    void addKRClicked();
    void updateKRTable();
    void krCellDoubleClicked(int row, int column);
    void krClearCellPressed();
    void editKRClicked();
    void deleteKRClicked();
    void receiptsTriggered();
    void verifyTriggered();
    void movesTriggered();
    void signersTriggered();
    void variablesTriggered();
    void krReportsTriggered();
    void trReportsTriggered();
    void matReportTriggered();
    void settingsSaved();
    void on_csAction_triggered();

protected:
    void showEvent(QShowEvent *event);

};
#endif // MAINWINDOW_H
