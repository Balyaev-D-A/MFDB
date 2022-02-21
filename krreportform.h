#ifndef KRREPORTFORM_H
#define KRREPORTFORM_H

#include <QWidget>

namespace Ui {
class KRReportForm;
}

class KRReportForm : public QWidget
{
    Q_OBJECT

public:
    explicit KRReportForm(QWidget *parent = nullptr);
    ~KRReportForm();

private:
    Ui::KRReportForm *ui;
};

#endif // KRREPORTFORM_H
