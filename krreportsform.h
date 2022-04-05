#ifndef KRREPORTSFORM_H
#define KRREPORTSFORM_H

#include <QWidget>
#include "database.h"
#include "krreportform.h"

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
    QStringList makeAVR(QString reportId);
    QStringList makeVVR(QString reportId);
    QStringList makeVFZM(QString reportId);
    QStringList makePO(QString reportId);
    QString minDate(QString date1, QString date2);
    QString maxDate(QString date1, QString date2);
    int maxInt(int first, int second);

protected:
    void showEvent(QShowEvent *event);

private slots:
    void addButtonClicked();
    void reportFormClosed(KRReportForm *sender);
    void updateReports();
    void editButtonClicked();
    void deleteButtonClicked();
    void saveButtonClicked();
};

#endif // KRREPORTSFORM_H
