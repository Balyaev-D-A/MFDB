#ifndef KRREPORTPREVIEWFORM_H
#define KRREPORTPREVIEWFORM_H

#include <QWidget>
#include <QWebEnginePage>
#include "database.h"

enum SignerType {
    OWNER = 0,
    MEMBER1,
    MEMBER2,
    MEMBER3,
    REPAIRER,
    CHIEF
};

namespace Ui {
class KRReportPreviewForm;
}

class KRReportPreviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit KRReportPreviewForm(QWidget *parent = nullptr);
    ~KRReportPreviewForm();
    void setDatabase(Database *db);
    void showPreview(QString reportId);

private:
    Ui::KRReportPreviewForm *ui;
    Database *db;
    int fillerBottom, fillerTop;
    int scriptResult;
    QWebEnginePage testpage;
    QString minDate(QString date1, QString date2);
    QString maxDate(QString date1, QString date2);
    void scriptCallback(const QVariant &result);
    QStringList makeAVR(QString reportId);
    QStringList makeVVR(QString reportId);
    QStringList makeVFZM(QString reportId);
};

#endif // KRREPORTPREVIEWFORM_H
