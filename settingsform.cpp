#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    settings = Settings::instance();
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::on_cancelButton_clicked()
{
    close();
}


void SettingsForm::on_saveButton_clicked()
{
    Settings::ConnSettings cs;
    cs.host = ui->hostEdit->text();
    cs.database = ui->databaseEdit->text();
    cs.user = ui->userEdit->text();
    cs.password = ui->passwordEdit->text();
    settings->setConnSettings(cs);
    settings->save();
    close();
    emit saved();
}

void SettingsForm::showEvent(QShowEvent *event)
{
    Settings::ConnSettings cs;
    cs = settings->getConnSettings();
    ui->hostEdit->setText(cs.host);
    ui->databaseEdit->setText(cs.database);
    ui->userEdit->setText(cs.user);
    ui->passwordEdit->setText(cs.password);
    QWidget::showEvent(event);
}
