#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "importform.h"
#include "ui_importform.h"
#include "csvreader.h"

ImportForm::ImportForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImportForm)
{
    ui->setupUi(this);
    connect(ui->fileButton, &QToolButton::clicked, this, &ImportForm::fileButtonClicked);
    connect(ui->readButton, &QPushButton::clicked, this, &ImportForm::readButtonClicked);
    connect(ui->importButton, &QPushButton::clicked, this, &ImportForm::importButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ImportForm::close);

    usedHeaders.clear();
}

ImportForm::~ImportForm()
{
    delete hcb;
    delete ui;
}

void ImportForm::fileButtonClicked()
{
    ui->fileEdit->setText(QFileDialog::getOpenFileName(this, "Открыть файл для импорта", ".", "Файлы CSV (*.csv)"));
}

void ImportForm::readButtonClicked()
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
    hHeader = ui->table->horizontalHeader();
    hHeader->setSectionsClickable(true);
    connect(ui->table->horizontalHeader(), &QHeaderView::sectionClicked, this, &ImportForm::headerSectionClicked);
    qDebug() << "Connected";
    reader.closeFile();
}

void ImportForm::cancelButtonClicked()
{
    close();
}

void ImportForm::importButtonClicked()
{
    close();
}

void ImportForm::setTable(QString table)
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

 void ImportForm::setDatabase(Database *db)
 {
     this->db = db;
 }

 void ImportForm::setHeaders(QStringList headers)
 {
     this->headers = headers;
 }

 void ImportForm::setFields(QStringList fields)
 {
     this->fields = fields;
 }

 void ImportForm::headerSectionClicked(int index)
 {
        ui->table->horizontalHeaderItem(index)->setText("clicked");
        return;
     hcb = new QComboBox(ui->table->horizontalHeader());
   //  hcb->hide();
     hcb->clear();
     hcb->addItem("-");
     for (int i = 1; i<headers.count(); i++)
     {
         if (!usedHeaders.contains(headers[i]))
             hcb->addItem(headers[i]);
     }
     hcb->setGeometry(ui->table->horizontalHeader()->sectionViewportPosition(index),
                      0, ui->table->horizontalHeader()->sectionSize(index),
                      ui->table->horizontalHeader()->height());
     hcb->show();
     hcb->setFocus();
 }

 void ImportForm::cbTextChoosed(QString text)
 {

 }

 void ImportForm::closeEvent(QCloseEvent *event)
 {
    event->accept();
    QWidget::closeEvent(event);
    emit closed();
 }
