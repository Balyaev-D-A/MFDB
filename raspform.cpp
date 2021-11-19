#include "raspform.h"
#include "ui_raspform.h"
#include <QMessageBox>
#include <QTextStream>

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

void RaspForm::setDatabase(Database *db)
{
    this->db = db;
}

void RaspForm::newRasp()
{
    ui->dateEdit->setDate(QDate::currentDate());

    db->pq->exec("select iss_id, iss_name, iss_default from issuers");
    while (db->pq->next())
        ui->issuerBox->addItem(db->pq->value(1).toString(), db->pq->value(0));
    db->pq->seek(-1);
    while (db->pq->next())
        if (db->pq->value(2).toBool()) {
            ui->issuerBox->setCurrentText(db->pq->value(1).toString());
            break;
        }
    db->pq->exec("select unit_id, unit_name from units order by unit_name");
    while (db->pq->next())
        ui->unitBox->addItem(db->pq->value(1).toString(), db->pq->value(0));
    ui->unitBox->setCurrentIndex(0);
    ui->monthBox->setCurrentIndex(QDate::currentDate().month());
    ui->newCheckBox->setChecked(true);
    ui->inworkCheckBox->setChecked(false);
    ui->completedCheckBox->setChecked(false);

    ui->teamTree->hideColumn(1);
    ui->currWorkTable->hideColumn(0);
    ui->workTable->hideColumn(0);
    updateWorkTable();
    connect(ui->unitBox, &QComboBox::currentTextChanged, this, &RaspForm::updateWorkTable);
    connect(ui->monthBox, &QComboBox::currentTextChanged, this, &RaspForm::updateWorkTable);
}

void RaspForm::updateWorkTable()
{
    QString query;
    QString month = "";

    while (ui->workTable->rowCount()>0) ui->workTable->removeRow(0);

    query = "select sch_id, sch_kks, sch_type, sch_worktype, loc_location, sch_hours, sch_state from schedule left join locations on schedule.sch_kks = locations.loc_kks "
            "where schedule.sch_executor = 'ИТЦРК' and sch_unit = '" + ui->unitBox->currentData().toString() + "' and sch_state < 2";
    if (ui->monthBox->currentIndex() != 0) {
        if (ui->monthBox->currentIndex() == 13){
            month = "ППР-" + QString::number(QDate::currentDate().year());
        } else {
            if (ui->monthBox->currentIndex() < 10) month = "0";
            month += QString::number(ui->monthBox->currentIndex()) + "." + QString::number(QDate::currentDate().year());
        }
    }
    if (month != "")
        query += "and sch_date = '" + month + "'";
//    QMessageBox::information(this, "Info", query);
    if (!db->pq->exec(query)) {
        QMessageBox::critical(this, "Ошибка выполнения запроса!", "Ошибка выполнения запроса: " + db->pq->lastError().text());
        return;
    }
    for (int i=0; db->pq->next(); i++)
    {
        ui->workTable->insertRow(i);
        for (int j=0; j<6; j++)
        {
            ui->workTable->setItem(i, j, new QTableWidgetItem(db->pq->value(j).toString()));
            if (db->pq->value(6) == 1)
                ui->workTable->item(i,j)->setBackground(QBrush(Qt::yellow));
        }
    }
    updateTotal();
}

void RaspForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed(this);
}

void RaspForm::updateTotal()
{
    float total = 0;
    QString labelText;
    for (int i=0; i<ui->workTable->rowCount(); i++ )
        total += ui->workTable->item(i,5)->text().toFloat();
    labelText = QString("Итоговые трудозатраты: %1 часов.").arg(QString::number(total, 'f', 2));
    ui->labelTotal->setText(labelText);
}

void RaspForm::updateRaspTotal()
{
    float total = 0;
    QString labelText;
    for (int i=0; i<ui->currWorkTable->rowCount(); i++)
        total += ui->currWorkTable->item(i, 5)->text().toFloat();
    labelText = QString("Итоговые трудозатраты: %1 часов.").arg(QString::number(total, 'f', 2));
    ui->labelRaspTotal->setText(labelText);
}
