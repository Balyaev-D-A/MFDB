#include "raspprintpreviewform.h"
#include "ui_raspprintpreviewform.h"

RaspPrintPreviewForm::RaspPrintPreviewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RaspPrintPreviewForm)
{
    ui->setupUi(this);
}

RaspPrintPreviewForm::~RaspPrintPreviewForm()
{
    delete ui;
}
