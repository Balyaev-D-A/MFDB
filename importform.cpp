#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include "importform.h"
#include "ui_importform.h"
#include "csvreader.h"

ImportForm::ImportForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ImportForm)
{
    ui->setupUi(this);
    connect(ui->fileButton, &QToolButton::clicked, this, &ImportForm::fileButtonClicked);
    connect(ui->readButton, &QPushButton::clicked, this, &ImportForm::readButtonClicked);
    connect(ui->importButton, &QPushButton::clicked, this, &ImportForm::importButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ImportForm::close);
    hcb.setParent(ui->table->horizontalHeader());
    hcb.hide();
    connect(&hcb, &QComboBox::currentTextChanged, this, &ImportForm::cbTextChanged);
}

ImportForm::~ImportForm()
{
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
    while (ui->table->rowCount()>0) ui->table->removeRow(0);
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
    reader.closeFile();
    /* ОРБшники сделали новый график с объединенными ячейками. Теперь нужно вставить в пустые ячейки,
     * которые образовались при разъединении, значения из ячейки выше.
     */

    for (int i=0; i<ui->table->columnCount(); i++)
        for (int j=1; j<ui->table->rowCount(); j++)
        {
            QString txt = ui->table->item(j, i)->text();
            if(ui->table->item(j, i)->text().isEmpty())
                ui->table->item(j, i)->setText(ui->table->item(j-1, i)->text());
        }
}

void ImportForm::cancelButtonClicked()
{
    close();
}

void ImportForm::importButtonClicked()
{
    QMap<QString, int> fieldsmap;
    QString field;
    QString query;
    QStringList usedfields;
    for (int i=0; i<ui->table->horizontalHeader()->count(); i++)
    {
        if (ui->table->horizontalHeaderItem(i)->text() != "-") {
            field = fields[headers.indexOf(ui->table->horizontalHeaderItem(i)->text())];
            fieldsmap[field] = i;
        }
    }
    db->startTransaction();
    usedfields = fieldsmap.keys();
    for (int i=0; i<ui->table->rowCount(); i++)
    {
        query = "INSERT INTO " + dbTable + " (";
        if (dbTable == "schedule")
            query += "sch_unit, ";
        query += db->explodeFields(usedfields, 0) + ") VALUES (";
        if (dbTable == "schedule")
            query += ui->unitBox->currentData().toString() + ", ";
        for (int j=0; j<usedfields.count(); j++)
        {
            query += "'" + ui->table->item(i,fieldsmap[usedfields[j]])->text() + "'";
            if (j!=usedfields.count()-1) query += ", ";
        }
        query +=")";
        if (!db->execQuery(query)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
    }
    db->commitTransaction();
    close();
}

void ImportForm::setTable(QString table)
{
    dbTable = table;
    if (table == "schedule") {
        if (db->execQuery("SELECT unit_name, unit_id FROM units ORDER BY unit_name")) {
            while (db->nextRecord())
            {
                ui->unitBox->addItem(db->fetchValue(0).toString(), db->fetchValue(1));
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
     comboCurrent = index;
     QStringList usedHeaders;
     int firstField;
     usedHeaders.clear();
     for (int i = 0; i<ui->table->horizontalHeader()->count(); i++)
     {
         if (ui->table->horizontalHeaderItem(i)->text() != "-")
             usedHeaders.append(ui->table->horizontalHeaderItem(i)->text());
     }

     hcb.clear();

     hcb.addItem("-");
     if (headers[0] == "id")
         firstField = 1;
     else
         firstField = 0;
     for (int i = firstField; i<headers.count(); i++)
     {
         if (i == 1 && dbTable == "schedule") continue;
         if (!usedHeaders.contains(headers[i]))
             hcb.addItem(headers[i]);
     }
     hcb.setGeometry(ui->table->horizontalHeader()->sectionViewportPosition(index),
                      0, ui->table->horizontalHeader()->sectionSize(index),
                      ui->table->horizontalHeader()->height());
     hcb.showPopup();
     hcb.setFocus();
 }

 void ImportForm::cbTextChanged(const QString &text)
 {
    ui->table->horizontalHeaderItem(comboCurrent)->setText(text);
    hcb.hide();
 }

 void ImportForm::closeEvent(QCloseEvent *event)
 {
    event->accept();
    QWidget::closeEvent(event);
    emit closed();
 }
