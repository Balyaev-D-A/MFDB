#ifndef KRREPORTSFORM_H
#define KRREPORTSFORM_H

#include <QWidget>
#include "database.h"
#include "krreportform.h"
#include "krreportpreviewform.h"

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

protected:
    void showEvent(QShowEvent *event);

private slots:
    void addButtonClicked();
    void reportFormClosed(KRReportForm *sender);
    void updateReports();
    void editButtonClicked();
    void deleteButtonClicked();
    void printButtonClicked();
    void printFormClosed(KRReportPreviewForm *sender);
};

#endif // KRREPORTSFORM_H
