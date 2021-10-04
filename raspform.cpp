#include "raspform.h"
#include "ui_raspform.h"

RaspForm::RaspForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RaspForm)
{
    ui->setupUi(this);

}

RaspForm::~RaspForm()
{
    delete ui;
}
