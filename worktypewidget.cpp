#include "worktypewidget.h"
#include "ui_worktypewidget.h"
#include <QFocusEvent>
#include <QShowEvent>

WorkTypeWidget::WorkTypeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkTypeWidget)
{
    ui->setupUi(this);
    connect(ui->TOBox, &WTCheckBox::stateChanged, this, &WorkTypeWidget::checkBoxStateChanged);
    connect(ui->TRBox, &WTCheckBox::stateChanged, this, &WorkTypeWidget::checkBoxStateChanged);
    connect(ui->KRBox, &WTCheckBox::stateChanged, this, &WorkTypeWidget::checkBoxStateChanged);
    connect(ui->MABox, &WTCheckBox::stateChanged, this, &WorkTypeWidget::checkBoxStateChanged);
    connect(ui->closeButton, &WTButton::clicked, this, &WorkTypeWidget::closeButtonClicked);
    connect(ui->TOBox, &WTCheckBox::focusLost, this, &WorkTypeWidget::childLostFocus);
    connect(ui->TRBox, &WTCheckBox::focusLost, this, &WorkTypeWidget::childLostFocus);
    connect(ui->KRBox, &WTCheckBox::focusLost, this, &WorkTypeWidget::childLostFocus);
    connect(ui->MABox, &WTCheckBox::focusLost, this, &WorkTypeWidget::childLostFocus);
    connect(ui->closeButton, &WTButton::focusLost, this, &WorkTypeWidget::childLostFocus);
    ui->TOBox->setFocus();
}

WorkTypeWidget::~WorkTypeWidget()
{
    delete ui;
}

void WorkTypeWidget::setWorkTypes(QString wt)
{
    ui->TOBox->blockSignals(true);
    ui->TRBox->blockSignals(true);
    ui->KRBox->blockSignals(true);
    ui->MABox->blockSignals(true);
    ui->TOBox->setChecked(false);
    ui->TRBox->setChecked(false);
    ui->KRBox->setChecked(false);
    ui->MABox->setChecked(false);

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

    ui->TOBox->blockSignals(false);
    ui->TRBox->blockSignals(false);
    ui->KRBox->blockSignals(false);
    ui->MABox->blockSignals(false);
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
        result += "МА";
    }
    if (result == "") result = "ТО";
    return result;
}


void WorkTypeWidget::checkBoxStateChanged()
{
    emit typeChanged();
}

void WorkTypeWidget::focusOutEvent(QFocusEvent *event)
{   
    if (!ui->TOBox->hasFocus() &
            !ui->TRBox->hasFocus() &
            !ui->KRBox->hasFocus() &
            !ui->MABox->hasFocus() &
            !ui->closeButton->hasFocus())
    {
        hide();
        emit widgetHidden(this);
    }
    QWidget::focusOutEvent(event);
}

void WorkTypeWidget::closeButtonClicked()
{
    hide();
    emit widgetHidden(this);
}

void WorkTypeWidget::childLostFocus()
{
    if (!ui->TOBox->hasFocus() &
            !ui->TRBox->hasFocus() &
            !ui->KRBox->hasFocus() &
            !ui->MABox->hasFocus() &
            !ui->closeButton->hasFocus() &
            !this->hasFocus())
    {
        hide();
        emit widgetHidden(this);
    }
}

void WorkTypeWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    ui->TOBox->setFocus();
}
