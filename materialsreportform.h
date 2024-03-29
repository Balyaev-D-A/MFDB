#ifndef MATERIALSREPORTFORM_H
#define MATERIALSREPORTFORM_H

#include <QWidget>
#include <QShowEvent>
#include <QCloseEvent>
#include "database.h"

namespace Ui {
class MaterialsReportForm;
}

class MaterialsReportForm : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialsReportForm(QWidget *parent = nullptr);
    ~MaterialsReportForm();
    void setDatabase(Database *db);

private:
    Ui::MaterialsReportForm *ui;
    Database *db;
    void updateTRMaterials();
    void updateKRMaterials();
    void updateMaterialBox();
private slots:
    void updateMaterials();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void closed();

};

#endif // MATERIALSREPORTFORM_H
