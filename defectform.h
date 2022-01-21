#ifndef DEFECTFORM_H
#define DEFECTFORM_H

#include <QWidget>

namespace Ui {
class DefectForm;
}

class DefectForm : public QWidget
{
    Q_OBJECT

public:
    explicit DefectForm(QWidget *parent = nullptr);
    ~DefectForm();

private:
    Ui::DefectForm *ui;
};

#endif // DEFECTFORM_H
