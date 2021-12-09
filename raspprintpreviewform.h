#ifndef RASPPRINTPREVIEWFORM_H
#define RASPPRINTPREVIEWFORM_H

#include <QWidget>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>
#include "database.h"

namespace Ui {
class RaspPrintPreviewForm;
}

class RaspPrintPreviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit RaspPrintPreviewForm(QWidget *parent = nullptr);
    ~RaspPrintPreviewForm();
    void setDatabase(Database *db);
    void showPreview(QStringList raspList);

private:
    Ui::RaspPrintPreviewForm *ui;
    QPrinter pdfPrinter;
    QPrinter paperPrinter;
    Database *db;
private slots:
    void saveToPDFClicked();
};

#endif // RASPPRINTPREVIEWFORM_H
