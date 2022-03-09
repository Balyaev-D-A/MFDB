#ifndef KRREPORTPREVIEWFORM_H
#define KRREPORTPREVIEWFORM_H

#include <QWidget>
#include <QWebEnginePage>
#include "database.h"

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
    void scriptCallback(const QVariant &result);
    QStringList makeAVR(QString reportId);
    QStringList makeVVR(QString reportId);
    QStringList makeVFZM(QString reportId);
};

#endif // KRREPORTPREVIEWFORM_H
