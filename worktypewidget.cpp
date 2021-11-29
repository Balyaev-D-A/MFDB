#include "worktypewidget.h"
#include "ui_worktypewidget.h"

WorkTypeWidget::WorkTypeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkTypeWidget)
{
    ui->setupUi(this);
}

WorkTypeWidget::~WorkTypeWidget()
{
    delete ui;
}

void WorkTypeWidget::setWorkTypes(QString wt)
{
    QStringList types = wt.split(",");
    for (int i=0; i<types.size(); i++)
    {
        if (types[i].simplified() == "ТО")
            ui->TOBox->setChecked(true);
        else if (types[i].simplified() == "ТР")
            ui->TRBox->setChecked(true);
        else if (types[i].simplified() == "КР")
            ui->KRBox->setChecked(true);
        else if (types[i].simplified() == "МА")
            ui->MABox->setChecked(true);
    }
}

QString WorkTypeWidget::workTypes()
{
    QString result = "";

    if (ui->TOBox->isChecked())
        result += "ТО";
    if (ui->TRBox->isChecked()) {
        if (result.size() > 0) result += ", ";
        result += "ТР";
    }
    if (ui->KRBox->isChecked()) {
        if (result.size() > 0) result += ", ";
        result += "КР";
    }
    if (ui->MABox->isChecked()) {
        if (result.size() > 0) result += ", ";
        result += "MA";
    }
    return ;
}
