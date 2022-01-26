#ifndef DEFECTFORM_H
#define DEFECTFORM_H

#include <QWidget>
#include "database.h"
#include "deviceselectorform.h"

typedef enum {
    EXTERNREVIEW = 0,
    EXTERNCONN = 1,
    INTERNCONN = 2,
    ELECTRICPAR = 3
} DefectStage;

typedef struct {
    int id;
    QString description;
    DefectStage stage;
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

private:
    Ui::DefectForm *ui;
    Database *db;
    DeviceSelectorForm *dsf;
    SelectedDevice device;
    DefectList defectList;
    RepairList repairList;
    void updateDefects();
    void updateRepairs();

private slots:
    void deviceButtonClicked();
    void selectorClosed(DeviceSelectorForm *sender);
    void deviceChoosed(SelectedDevice device);
};

#endif // DEFECTFORM_H
