#ifndef DEFECTFORM_H
#define DEFECTFORM_H

#include <QWidget>
#include "database.h"
#include "deviceselectorform.h"


const int EXTERNREVIEW = 0;
const int EXTERNCONN = 1;
const int INTERNCONN = 2;
const int ELECTRICPAR = 3;


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
    void newDefect();
    void editDefect(QString defId);

private:
    Ui::DefectForm *ui;
    Database *db;
    DeviceSelectorForm *dsf;
    SelectedDevice device;
    DefectList defectList;
    RepairList repairList;
    QString defId;
    bool matsChanged;
    int currentDefect;
    int currentRepair;
    void updateDefects();
    void updateRepairs();
    void updateDefectText();
    void updateRepairText();
    void updateAddedMaterials();
    void updateMaterials();
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

};

#endif // DEFECTFORM_H
