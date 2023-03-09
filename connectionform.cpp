#include "connectionform.h"
#include "ui_connectionform.h"
#include "settings.h"

ConnectionForm::ConnectionForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ConnectionForm)
{
    ui->setupUi(this);
    Settings::ConnSettings cs;
    cs = Settings::instance()->getConnSettings();
    ui->hostEdit->setText(cs.host);
    ui->databaseEdit->setText(cs.database);
    ui->userEdit->setText(cs.user);
    ui->passwordEdit->setText(cs.password);
    setWindowModality(Qt::WindowModal);
}

ConnectionForm::~ConnectionForm()
{
    delete ui;
}

void ConnectionForm::on_cancelButton_clicked()
{
    close();
}


void ConnectionForm::on_connectButton_clicked()
{
    Settings::ConnSettings cs;
    cs.host = ui->hostEdit->text();
    cs.database = ui->databaseEdit->text();
    cs.user = ui->userEdit->text();
    cs.password = ui->passwordEdit->text();
    close();
    emit connectClicked(cs);
}

