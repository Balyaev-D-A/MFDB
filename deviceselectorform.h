#ifndef DEVICESELECTORFORM_H
#define DEVICESELECTORFORM_H

#include <QWidget>

namespace Ui {
class DeviceSelectorForm;
}

class DeviceSelectorForm : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceSelectorForm(QWidget *parent = nullptr);
    ~DeviceSelectorForm();

private:
    Ui::DeviceSelectorForm *ui;
};

#endif // DEVICESELECTORFORM_H
