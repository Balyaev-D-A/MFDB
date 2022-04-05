#ifndef KRREPORTFORM_H
#define KRREPORTFORM_H

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
} KRSigners;

enum KRSignerType {
    KRSOWNER = 0,
    KRSMEMBER1,
    KRSMEMBER2,
    KRSMEMBER3,
    KRSMEMBER4,
    KRSMEMBER5,
    KRSREPAIRER,
    KRSCHIEF
};

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
    void editReport(QString Id);

private:
    Ui::KRReportForm *ui;
    Database *db;
    KRSigners signers;
    QString reportId = "0";
    int unitId = -1;
    void fillUnitBox();
    bool checkFilling();
    void updateSignersTable();
    void showErrorMessage(QString message);;
private slots:
    void updateKRTable();
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
    void closed(KRReportForm *sender);
    void saved();
};

#endif // KRREPORTFORM_H
