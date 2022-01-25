#include "deviceselectorform.h"
#include "ui_deviceselectorform.h"

DeviceSelectorForm::DeviceSelectorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceSelectorForm)
{
    ui->setupUi(this);
}

DeviceSelectorForm::~DeviceSelectorForm()
{
    delete ui;
}
