#ifndef KRREPORTSFORM_H
#define KRREPORTSFORM_H

#include <QWidget>

namespace Ui {
class KRReportsForm;
}

class KRReportsForm : public QWidget
{
    Q_OBJECT

public:
    explicit KRReportsForm(QWidget *parent = nullptr);
    ~KRReportsForm();

private:
    Ui::KRReportsForm *ui;
};

#endif // KRREPORTSFORM_H
