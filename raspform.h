#ifndef RASPFORM_H
#define RASPFORM_H

#include <QWidget>

namespace Ui {
class RaspForm;
}

class RaspForm : public QWidget
{
    Q_OBJECT

public:
    explicit RaspForm(QWidget *parent = nullptr);
    ~RaspForm();


private:
    Ui::RaspForm *ui;
};

#endif // RASPFORM_H
