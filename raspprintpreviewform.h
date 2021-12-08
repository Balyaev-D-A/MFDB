#ifndef RASPPRINTPREVIEWFORM_H
#define RASPPRINTPREVIEWFORM_H

#include <QWidget>

namespace Ui {
class RaspPrintPreviewForm;
}

class RaspPrintPreviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit RaspPrintPreviewForm(QWidget *parent = nullptr);
    ~RaspPrintPreviewForm();

private:
    Ui::RaspPrintPreviewForm *ui;
};

#endif // RASPPRINTPREVIEWFORM_H
