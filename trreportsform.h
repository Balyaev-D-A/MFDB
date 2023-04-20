#ifndef TRREPORTSFORM_H
#define TRREPORTSFORM_H

#include <QWidget>
#include <database.h>

#include "trreportform.h"

namespace Ui {
class TRReportsForm;
}

class TRReportsForm : public QWidget
{
    Q_OBJECT

public:
    explicit TRReportsForm(QWidget *parent = nullptr);
    ~TRReportsForm();
    void setDatabase(Database *db);

private:
    Ui::TRReportsForm *ui;
    Database *db;
    QString lastSavedDir = "";
    QStringList makeAVR(QString reportId);
    QStringList makeVVR(QString reportId);
    QStringList makeVFZM(QString reportId);
    QStringList makeADO(QString reportId);
    QStringList makePO(QString reportId);
    QString minDate(QString date1, QString date2);
    QString maxDate(QString date1, QString date2);
    int maxInt(int first, int second);
    QString makeJson(QString reportId);


private slots:
    void addButtonClicked();
    void reportFormClosed(TRReportForm *sender);
    void updateReports();
    void editButtonClicked();
    void deleteButtonClicked();
    void saveButtonClicked();
    void tableCellDoubleClicked(int row, int column);

protected:
    void showEvent(QShowEvent *event);
};

#endif // TRREPORTSFORM_H
