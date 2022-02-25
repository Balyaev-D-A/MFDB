#ifndef KRREPORTFORM_H
#define KRREPORTFORM_H

#include <QWidget>
#include "database.h"

namespace Ui {
class KRReportForm;
}

class KRReportForm : public QWidget
{
    Q_OBJECT

public:
    explicit KRReportForm(QWidget *parent = nullptr);
    ~KRReportForm();
    void setDatabase(Database *db);
    void newReport();
    void editReport();

private:
    Ui::KRReportForm *ui;
    Database *db;
    void fillUnitBox();
private slots:
    void updateKRTable();
    void addButtonClicked();
    void removeButtonClicked();
    void unitChanged();
protected:
    void showEvent(QShowEvent *event);
};

#endif // KRREPORTFORM_H
