#ifndef KRREPORTFORM_H
#define KRREPORTFORM_H

#include <QWidget>
#include "database.h"

typedef  struct {
    QString ownerId = "";
    QString member1Id = "";
    QString member2Id = "";
    QString member3Id = "";
    QString repairerId = "";
    QString chiefId = "";
} Signers;

enum SignerType {
    OWNER = 0,
    MEMBER1,
    MEMBER2,
    MEMBER3,
    REPAIRER,
    CHIEF
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
    Signers signers;
    QString reportId = "0";
    int unitId = -1;
    void fillUnitBox();
    bool checkFilling();
    void updateSignersTable();
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
    void repairerDroped();
    void chiefDroped();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void closed(KRReportForm *sender);
    void saved();
};

#endif // KRREPORTFORM_H
