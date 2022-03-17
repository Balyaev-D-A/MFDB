#include "krreportsform.h"
#include "ui_krreportsform.h"

KRReportsForm::KRReportsForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::KRReportsForm)
{
    ui->setupUi(this);
    ui->table->hideColumn(0);
    connect(ui->addButton, &QToolButton::clicked, this, &KRReportsForm::addButtonClicked);
    connect(ui->updateButton, &QToolButton::clicked, this, &KRReportsForm::updateReports);
    connect(ui->editButton, &QToolButton::clicked, this, &KRReportsForm::editButtonClicked);
    connect(ui->deleteButton, &QToolButton::clicked, this, &KRReportsForm::deleteButtonClicked);
    connect(ui->printButton, &QToolButton::clicked, this, &KRReportsForm::printButtonClicked);

}

KRReportsForm::~KRReportsForm()
{
    delete ui;
}

void KRReportsForm::setDatabase(Database *db)
{
    this->db = db;
}

void KRReportsForm::updateReports()
{
    int curRow;
    QString query = "SELECT krr_id, unit_name, krr_desc FROM krreports "
                    "LEFT JOIN units ON krr_unit = unit_id";

    while (ui->table->rowCount()) ui->table->removeRow(0);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    ui->table->setSortingEnabled(false);
    while (db->nextRecord())
    {
        curRow = ui->table->rowCount();
        ui->table->insertRow(curRow);
        for (int i=0; i<3; i++)
        {
            ui->table->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->table->setSortingEnabled(true);
    ui->table->resizeColumnsToContents();
}

void KRReportsForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateReports();
}

void KRReportsForm::addButtonClicked()
{
    KRReportForm *krf = new KRReportForm(this);
    krf->setDatabase(db);
    connect(krf, &KRReportForm::closed, this, &KRReportsForm::reportFormClosed);
    connect(krf, &KRReportForm::saved, this, &KRReportsForm::updateReports);
    krf->show();
}

void KRReportsForm::reportFormClosed(KRReportForm *sender)
{
    disconnect(sender, &KRReportForm::closed, this, &KRReportsForm::reportFormClosed);
    disconnect(sender, &KRReportForm::saved, this, &KRReportsForm::updateReports);
    sender->deleteLater();
}

void KRReportsForm::editButtonClicked()
{
    if (!ui->table->currentItem()) return;
    KRReportForm *krf = new KRReportForm(this);
    krf->setDatabase(db);
    krf->editReport(ui->table->item(ui->table->currentRow(), 0)->text());
    connect(krf, &KRReportForm::closed, this, &KRReportsForm::reportFormClosed);
    connect(krf, &KRReportForm::saved, this, &KRReportsForm::updateReports);
    krf->show();
}

void KRReportsForm::deleteButtonClicked()
{
    if (!ui->table->currentItem()) return;

    QString query = "DELETE FROM krreports WHERE krr_id = '%1'";
    query = query.arg(ui->table->item(ui->table->currentRow(), 0)->text());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    ui->table->removeRow(ui->table->currentRow());
}

void KRReportsForm::printButtonClicked()
{
    if (!ui->table->currentItem()) return;

    KRReportPreviewForm *krp = new KRReportPreviewForm(this);
    connect(krp, &KRReportPreviewForm::closed, this, &KRReportsForm::printFormClosed);
    krp->setDatabase(db);
    krp->showPreview(ui->table->item(ui->table->currentRow(), 0)->text());
}

void KRReportsForm::printFormClosed(KRReportPreviewForm *sender)
{
    disconnect(sender, &KRReportPreviewForm::closed, this, &KRReportsForm::printFormClosed);
    sender->deleteLater();
}
