#ifndef KRREPORTSFORM_H
#define KRREPORTSFORM_H

#include <QWidget>
#include "database.h"

namespace Ui {
class KRReportsForm;
}

class KRReportsForm : public QWidget
{
    Q_OBJECT

public:
    explicit KRReportsForm(QWidget *parent = nullptr);
    ~KRReportsForm();
    void setDatabase(Database *db);

private:
    Ui::KRReportsForm *ui;
    Database *db;
    void addButtonClicked();
};

#endif // KRREPORTSFORM_H
