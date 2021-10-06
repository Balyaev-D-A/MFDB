#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>

#include "dictionaryform.h"
#include "ui_dictionaryform.h"
#include "importform.h"


DictionaryForm::DictionaryForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DictionaryForm)
{
    ui->setupUi(this);
    ui->table->setShowGrid(true);
    ui->table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setEditTriggers(0);
    editor = new FieldEditor(ui->table->viewport());
    editor->hide();
    connect(ui->table, &QTableWidget::cellDoubleClicked, this, &DictionaryForm::cellDoubleClicked);
    connect(editor, &FieldEditor::rejectInput, this, &DictionaryForm::inputRejected);
    connect(editor, &FieldEditor::acceptInput, this, &DictionaryForm::inputAccepted);
    connect(ui->addBtn, &QToolButton::clicked, this, &DictionaryForm::addRecord);
    connect(ui->deleteBtn, &QToolButton::clicked, this, &DictionaryForm::deleteRecord);
    connect(ui->updateBtn, &QToolButton::clicked, this, &DictionaryForm::updateData);
    connect(ui->importBtn, &QToolButton::clicked, this, &DictionaryForm::importBtnClicked);
}

DictionaryForm::~DictionaryForm()
{
    delete editor;
    delete ui;
}

void DictionaryForm::setDatabase(Database *db)
{
    this->db = db;
}

void DictionaryForm::setDictionary(Dictionary dictionary)
{
    dict = dictionary;
    ui->table->clear();
    fields.clear();
    fieldTypes.clear();
    headers.clear();
    switch (dict) {
    case EMPLOYEES:
        setWindowTitle("Справочник: Работники");
        dbTable = "employees";
        fields.append("emp_id");
        fields.append("emp_num");
        fields.append("emp_name");
        fields.append("emp_position");
        fields.append("emp_group");
        fields.append("emp_tld");
        fields.append("emp_admin");
        fields.append("emp_metrolog");
        fields.append("emp_hidden");
        fieldTypes.append("int");
        fieldTypes.append("int");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("int1");
        fieldTypes.append("int");
        fieldTypes.append("bool");
        fieldTypes.append("bool");
        fieldTypes.append("bool");
        headers.append("id");
        headers.append("Табельный номер");
        headers.append("Фамилия И. О.");
        headers.append("Должность");
        headers.append("Группа ЭБ");
        headers.append("Номер ТЛД");
        headers.append("Админ");
        headers.append("Метролог");
        headers.append("Скрыт");
        break;
    case SCHEDULE:
        setWindowTitle("Справочник: График работ");
        dbTable = "schedule";
        fields.append("sch_id");
        fields.append("sch_unit");
        fields.append("sch_kks");
        fields.append("sch_name");
        fields.append("sch_type");
        fields.append("sch_tdoc");
        fields.append("sch_date");
        fields.append("sch_reportdate");
        fields.append("sch_worktype");
        fields.append("sch_hours");
        fields.append("sch_executor");
        fields.append("sch_state");
        fieldTypes.append("int");
        fieldTypes.append("int");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("real");
        fieldTypes.append("string");
        fieldTypes.append("int");
        headers.append("id");
        headers.append("Блок");
        headers.append("Обозначение");
        headers.append("Наименование");
        headers.append("Тип");
        headers.append("Техдокумент");
        headers.append("Дата работ");
        headers.append("Дата отчета");
        headers.append("Тип работ");
        headers.append("Трудозатраты");
        headers.append("Исполнитель");
        headers.append("Состояние");
        break;

    case UNITS:
        setWindowTitle("Справочник: Блоки");
        dbTable = "units";
        fields.append("unit_id");
        fields.append("unit_name");
        fieldTypes.append("int");
        fieldTypes.append("string");
        headers.append("id");
        headers.append("Наименование");
        break;
    case LOCATIONS:
        setWindowTitle("Справочник: Размещение оборудования");
        dbTable = "locations";
        fields.append("loc_kks");
        fields.append("loc_location");
        fieldTypes.append("string");
        fieldTypes.append("string");
        headers.append("Обозначение");
        headers.append("Размещение");
        break;
    }

    ui->table->setColumnCount(fields.count());
    ui->table->horizontalHeader()->setFont(QFont(QFont().defaultFamily(), -1, QFont::Bold));
    ui->table->setHorizontalHeaderLabels(headers);
    if (headers[0] == "id")
        ui->table->hideColumn(0);
    ui->table->verticalHeader()->hide();
    ui->table->setSortingEnabled(true);
    updateData();
}

void DictionaryForm::updateData()
{
    while (ui->table->rowCount()>0) ui->table->removeRow(0);
    db->pq->exec("select " + db->explodeFields(fields, 0) + " from " + dbTable);
    for (int i = 0; db->pq->next(); i++)
    {
        ui->table->insertRow(i);
        for (int j=0; j<fields.count(); j++){
            if (fieldTypes.at(j) == "bool") {
                QTableWidgetItem *it = new QTableWidgetItem();
                it->data(Qt::CheckStateRole);
                it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                if (db->pq->value(j).toBool())
                    it->setCheckState(Qt::Checked);
                else
                    it->setCheckState(Qt::Unchecked);
                ui->table->setItem(i, j, it);
            } else
                ui->table->setItem(i, j, new QTableWidgetItem(db->pq->value(j).toString()));
        }
    }
    ui->table->resizeColumnsToContents();
    if (headers[0] == "id")
        ui->table->sortItems(1);
    else
        ui->table->sortItems(0);
    ui->table->selectRow(0);
}

void DictionaryForm::addRecord()
{
    QVariant lId;
    if (db->pq->exec("insert into " + dbTable + " default values" )) {
        lId = db->pq->lastInsertId();
       if (lId.isValid()) {
            ui->table->insertRow(ui->table->rowCount());
            ui->table->setCurrentCell(ui->table->rowCount()-1, 1);
            for (int i=0; i<fields.size(); i++)
            {
                if (i == 0)
                    ui->table->setItem(ui->table->rowCount()-1, 0, new QTableWidgetItem(lId.toString()));
                else if (fieldTypes.at(i) == "bool") {
                    QTableWidgetItem *it = new QTableWidgetItem();
                    it->data(Qt::CheckStateRole);
                    it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    it->setCheckState(Qt::Unchecked);
                    ui->table->setItem(ui->table->rowCount()-1, i, it);
                } else
                    ui->table->setItem(ui->table->rowCount()-1, i, new QTableWidgetItem(""));
            }

        }
    } else {
        qDebug() << "Ошибка выполнения запроса";
}
}

void DictionaryForm::deleteRecord()
{
    int cr = ui->table->currentRow();
    if (cr<0) return;
    QString crId = ui->table->item(cr,0)->text();

    if (db->pq->exec("delete from " + dbTable + " where " + fields.at(0) + "=" + crId))
        ui->table->removeRow(cr);
    else
        qDebug() << "Ошибка выполнения запроса";//TODO: Обработка ошибки
}

void DictionaryForm::cellDoubleClicked(int row, int column)
{
    QString crId = ui->table->item(row, 0)->text();
    if (fieldTypes.at(column) == "bool") {
        bool cs = (ui->table->item(row, column)->checkState() == Qt::Checked);
        if(db->pq->exec("update " + dbTable + " set " + fields.at(column) + "=" + QVariant(!cs).toString() + " where " + fields.at(0) + "=" + crId)) {
            if (cs)
                ui->table->item(row, column)->setCheckState(Qt::Unchecked);
            else
                ui->table->item(row, column)->setCheckState(Qt::Checked);
        }
        else {
            return; //TODO: Обработка ошибки
        }
    }
    else if ((fieldTypes.at(column) == "int") || (fieldTypes.at(column) == "int1")) {
        editor->setGeometry(ui->table->visualItemRect(ui->table->item(row, column)));
        if (fieldTypes.at(column) == "int1")
            editor->setType(EDIGIT);
        else
            editor->setType(EINT);
        editor->setCell(row, column);
        editor->setText(ui->table->item(row, column)->text());
        editor->selectAll();
        editor->show();
        editor->setFocus();
    }
    else {
        editor->setGeometry(ui->table->visualItemRect(ui->table->item(row, column)));
        editor->setType(ESTRING);
        editor->setMaxLength(255);
        editor->setCell(row, column);
        editor->setText(ui->table->item(row, column)->text());
        editor->selectAll();
        editor->show();
        editor->setFocus();
    }
}

void DictionaryForm::inputRejected()
{
    editor->hide();
    ui->table->setFocus();
}

void DictionaryForm::inputAccepted()
{
    QString crId = ui->table->item(editor->getRow(), 0)->text();
   qDebug() << "update " + dbTable + " set " + fields.at(editor->getColumn()) + "='" +editor->text() + "' where " + fields.at(0) + "=" + crId;
    if (db->pq->exec("update " + dbTable + " set " + fields.at(editor->getColumn()) + "='" +
                     editor->text() + "'where " + fields.at(0) + "=" + crId)){
        ui->table->item(editor->getRow(), editor->getColumn())->setText(editor->text());
        ui->table->resizeColumnToContents(editor->getColumn());
    } else
        qDebug() << "Ошибка выполнения запроса: " + db->lastError().databaseText();//TODO: Обработка ошибки

    editor->hide();
    ui->table->setFocus();
}

void DictionaryForm::importBtnClicked()
{
    importForm = new ImportForm();

    importForm->setDatabase(db);
    importForm->setHeaders(headers);
    importForm->setFields(fields);
    importForm->setTable(dbTable);
    connect(importForm, &ImportForm::closed, this, &DictionaryForm::importFormClosed);
    importForm->show();
}

void DictionaryForm::importFormClosed()
{
    disconnect(importForm, &ImportForm::closed, this, &DictionaryForm::importFormClosed);
    delete importForm;
    importForm = nullptr;
    updateData();
}
