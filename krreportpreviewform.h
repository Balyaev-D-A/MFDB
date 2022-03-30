#ifndef KRREPORTPREVIEWFORM_H
#define KRREPORTPREVIEWFORM_H

#include <QWidget>
#include <QWebEnginePage>
#include "database.h"
#include "krreportform.h"

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
    QString currentPage;
    int fillerBottom, fillerTop;
    int scriptResult;
    QWebEnginePage testpage;
    QString minDate(QString date1, QString date2);
    QString maxDate(QString date1, QString date2);
    void scriptCallback(const QVariant &result);
    QStringList makeAVR(QString reportId);
    QStringList makeVVR(QString reportId);
    QStringList makeVFZM(QString reportId);
    QStringList makePO(QString reportId);
    int maxInt(int first, int second);

private slots:
    void saveButtonClicked();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed(KRReportPreviewForm *sender);
};

#endif // KRREPORTPREVIEWFORM_H
