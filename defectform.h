#ifndef DEFECTFORM_H
#define DEFECTFORM_H

#include <QWidget>
#include "database.h"

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
};

#endif // DEFECTFORM_H
