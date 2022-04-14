#ifndef KRFORM_H
#define KRFORM_H

#include <QWidget>
#include "database.h"
#include "krselectorform.h"
#include "normativeform.h"
#include "fieldeditor.h"

namespace Ui {
class KRForm;
}


class KRForm : public QWidget
{
    Q_OBJECT

public:
    explicit KRForm(QWidget *parent = nullptr);
    ~KRForm();
    void setDatabase(Database *db);
    void setNormativeForm(NormativeForm *nf);
    void newKR();
    void editKR(QString KRId);

private:
    Ui::KRForm *ui;
    Database *db;
    NormativeForm *nf;
    QString KRId;
    QString selectedSched;
    QString selectedDevice;
    bool matsChanged;
    void updateMaterials();
    void updateAddedMats();
    bool saveKR();

private slots:
    void selectDeviceClicked();
    void addMaterialClicked();
    void removeMaterialClicked();
    void cancelClicked();
    void okClicked();
    void selectorClosed(KRSelectorForm *sender);
    void deviceSelected(const KRDevice &device);
    void oesnClicked();
    void cellDoubleClicked(int row, int column);
    void inputAccepted(FieldEditor *editor);
    void inputRejected(FieldEditor *editor);
    void normativeSaved(QString device, QString workType);

signals:
    void krSaved();
};

#endif // KRFORM_H
