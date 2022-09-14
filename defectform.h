#ifndef DEFECTFORM_H
#define DEFECTFORM_H

#include <QWidget>
#include <QKeyEvent>
#include "database.h"
#include "deviceselectorform.h"
#include "normativeform.h"

class FieldEditor;

typedef struct {
    int id;
    QString description;
    int stage;
} Defect;

typedef QList<Defect> DefectList;

typedef struct {
    int id;
    QString description;
} Repair;

typedef QList<Repair> RepairList;

namespace Ui {
class DefectForm;
}

class DefectForm : public QWidget
{
    Q_OBJECT

public:
    explicit DefectForm(QWidget *parent = nullptr);
    ~DefectForm();
    void setDatabase(Database *db);
    void newDefect(int quarter);
    void editDefect(QString defId);
    void setNormativeForm(NormativeForm *nf);

private:
    Ui::DefectForm *ui;
    Database *db;
    NormativeForm *nf;
    DeviceSelectorForm *dsf;
    SelectedDevice device;
    DefectList defectList;
    RepairList repairList;
    QString defId;
    QString actions;
    bool matsChanged;
    int currentDefect;
    int currentRepair;
    void updateDefects();
    void updateRepairs();
    void updateDefectText();
    void updateRepairText();
    void updateAddedMaterials();
    void updateMaterials();
    void updateStages();
    void updateActions();
    void updateActionsDesc();
    bool saveDefect();

private slots:
    void deviceButtonClicked();
    void selectorClosed(DeviceSelectorForm *sender);
    void deviceChoosed(SelectedDevice device);
    void addDefectClicked();
    void saveDefectClicked();
    void deleteDefectClicked();
    void addRepairClicked();
    void deleteRepairClicked();
    void saveRepairClicked();
    void prevDefectClicked() {currentDefect--; updateDefectText(); updateRepairs();}
    void nextDefectClicked() {currentDefect++; updateDefectText(); updateRepairs();}
    void prevRepairClicked() {currentRepair--; updateRepairText();}
    void nextRepairClicked() {currentRepair++; updateRepairText();}
    void addMaterialClicked();
    void removeMaterialClicked();
    void okClicked();
    void cancelClicked();
    void oesnClicked();
    void cellDoubleClicked(int row, int column);
    void inputAccepted(FieldEditor *editor);
    void inputRejected(FieldEditor *editor);
    void normativeSaved(QString device, QString workType);
    void fillButtonClicked();
    void defectTextChanged();
    void repairTextChanged();

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void defectSaved();

};

#endif // DEFECTFORM_H
