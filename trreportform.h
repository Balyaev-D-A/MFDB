#ifndef TRREPORTFORM_H
#define TRREPORTFORM_H

#include <QWidget>
#include "database.h"


typedef  struct {
    QString ownerId = "";
    QString member1Id = "";
    QString member2Id = "";
    QString member3Id = "";
    QString member4Id = "";
    QString member5Id = "";
    QString repairerId = "";
    QString chiefId = "";
} TRSigners;

enum TRSignerType {
    TRSOWNER = 0,
    TRSMEMBER1,
    TRSMEMBER2,
    TRSMEMBER3,
    TRSMEMBER4,
    TRSMEMBER5,
    TRSREPAIRER,
    TRSCHIEF
};

namespace Ui {
class TRReportForm;
}

class TRReportForm : public QWidget
{
    Q_OBJECT

public:
    explicit TRReportForm(QWidget *parent = nullptr);
    ~TRReportForm();
    void setDatabase(Database *db);
    void editReport(QString Id);

private:
    Ui::TRReportForm *ui;
    Database *db;
    TRSigners signers;
    QString reportId = "0";
    int unitId = -1;
    void fillUnitBox();
    void updateSignersTable();
    bool checkFilling();
    void showErrorMessage(QString message);
    QString maxDate(QString date1, QString date2);

private slots:
    void updateTRTable();
    void addButtonClicked();
    void removeButtonClicked();
    void addAllButtonClicked();
    void removeAllButtonClicked();
    void unitChanged();
    void nextButtonClicked();
    void backButtonClicked();
    void doneButtonClicked();
    void ownerDroped();
    void member1Droped();
    void member2Droped();
    void member3Droped();
    void member4Droped();
    void member5Droped();
    void repairerDroped();
    void chiefDroped();
    void ownerClearClicked();
    void member1ClearClicked();
    void member2ClearClicked();
    void member3ClearClicked();
    void member4ClearClicked();
    void member5ClearClicked();
    void repairerClearClicked();
    void chiefClearClicked();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void closed(TRReportForm *sender);
    void saved();
};

#endif // TRREPORTFORM_H
