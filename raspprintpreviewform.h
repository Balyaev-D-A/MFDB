#ifndef RASPPRINTPREVIEWFORM_H
#define RASPPRINTPREVIEWFORM_H

#include <QWidget>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>

namespace Ui {
class RaspPrintPreviewForm;
}

class RaspPrintPreviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit RaspPrintPreviewForm(QWidget *parent = nullptr);
    ~RaspPrintPreviewForm();
    void showPreview(QStringList raspList);

private:
    Ui::RaspPrintPreviewForm *ui;
    QPrinter pdfPrinter;
    QPrinter paperPrinter;
};

#endif // RASPPRINTPREVIEWFORM_H
