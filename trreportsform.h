#ifndef TRREPORTSFORM_H
#define TRREPORTSFORM_H

#include <QWidget>

namespace Ui {
class TRReportsForm;
}

class TRReportsForm : public QWidget
{
    Q_OBJECT

public:
    explicit TRReportsForm(QWidget *parent = nullptr);
    ~TRReportsForm();

private:
    Ui::TRReportsForm *ui;
};

#endif // TRREPORTSFORM_H
