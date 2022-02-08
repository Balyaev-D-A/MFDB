#ifndef KRFORM_H
#define KRFORM_H

#include <QWidget>
#include "database.h"
#include "krselectorform.h"


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
    void newKR();
    void editKR(QString KRId);

private:
    Ui::KRForm *ui;
    Database *db;
    QString KRId;
    QString selectedSched;
    bool matsChanged;
    void updateMaterials();
    bool saveKR();

private slots:
    void selectDeviceClicked();
    void addMaterialClicked();
    void removeMaterialClicked();
    void cancelClicked();
    void okClicked();
    void selectorClosed(KRSelectorForm *sender);
    void deviceSelected(const KRDevice &device);

signals:
    void krSaved();
};

#endif // KRFORM_H
