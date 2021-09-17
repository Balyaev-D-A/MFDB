#include <QFileDialog>
#include <QMessageBox>
#include "importdialog.h"
#include "ui_importdialog.h"
#include "csvreader.h"

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);
    connect(ui->fileButton, &QToolButton::clicked, this, &ImportDialog::fileButtonClicked);
    connect(ui->readButton, &QPushButton::clicked, this, &ImportDialog::readButtonClicked);
    connect(ui->importButton, &QPushButton::clicked, this, &ImportDialog::importButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ImportDialog::reject);
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::fileButtonClicked()
{
    ui->fileEdit->setText(QFileDialog::getOpenFileName(this, "Открыть файл для импорта", ".", "Файлы CSV (*.csv)"));
}

void ImportDialog::readButtonClicked()
{
    CSVReader reader;
    CSVRow row;

    if (!reader.openFile(ui->fileEdit->text())) {
        QMessageBox::critical(this, "Ошибка!!!", "Ошибка открытия файла!");
        return;
    }

    while (!reader.atEnd())
    {
        row = reader.readNextRow();
        if (ui->table->columnCount() == 0)
            ui->table->setColumnCount(row.count());
        ui->table->insertRow(ui->table->rowCount());
        for (int i=0; i<row.count(); i++)
            ui->table->setItem(ui->table->rowCount()-1, i, new QTableWidgetItem(row[i]));
    }
    for (int i=0; i<ui->table->columnCount(); i++)
    {
        ui->table->setHorizontalHeaderItem(i, new QTableWidgetItem("-"));
    }
    reader.closeFile();
}

void ImportDialog::cancelButtonClicked()
{
    reject();
}

void ImportDialog::importButtonClicked()
{
    accept();
}

void ImportDialog::setTable(QString table)
{
    dbTable = table;
    if (table == "schedule") {
        if (db->pq->exec("select unit_name, unit_id from units order by unit_name")) {
            while (db->pq->next())
            {
                ui->unitBox->addItem(db->pq->value(0).toString(), db->pq->value(1));
            }
        }
    }
    else {
        ui->unitBox->hide();
        ui->unitLabel->hide();
    }
}

 void ImportDialog::setDatabase(Database *db)
 {
     this->db = db;
 }

 void ImportDialog::setHeaders(QStringList headers)
 {
     this->headers = headers;
 }

 void ImportDialog::setFields(QStringList fields)
 {
     this->fields = fields;
 }
