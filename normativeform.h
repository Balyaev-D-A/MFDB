#ifndef NORMATIVEFORM_H
#define NORMATIVEFORM_H

#include <QWidget>
#include <QShowEvent>
#include "database.h"

namespace Ui {
class NormativeForm;
}

class NormativeForm : public QWidget
{
    Q_OBJECT

public:
    explicit NormativeForm(QWidget *parent = nullptr);
    ~NormativeForm();
    void setDatabase(Database *db);

private:
    Ui::NormativeForm *ui;
    Database *db;
    void updateNormTable();
    void updateMatTable();
    void updateNormatives();

private slots:
    bool saveNormatives();
    void addMatClicked();
    void removeMatClicked();
    void okClicked();


protected:
    void showEvent(QShowEvent *event);
};

#endif // NORMATIVEFORM_H
