#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QProgressDialog>

#include "dictionaryform.h"
#include "ui_dictionaryform.h"
#include "importform.h"


DictionaryForm::DictionaryForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
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
    connect(ui->copyButton, &QToolButton::clicked, this, &DictionaryForm::copyRecord);
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
        fields.append("sch_invno");
        fields.append("sch_name");
        fields.append("sch_type");
        fields.append("sch_tdoc");
        fields.append("sch_date");
        fields.append("sch_reportdate");
        fields.append("sch_worktype");
        fields.append("sch_hours");
        fields.append("sch_executor");
        fields.append("sch_done");
        fields.append("sch_note");
        fieldTypes.append("int");
        fieldTypes.append("int");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("real");
        fieldTypes.append("string");
        fieldTypes.append("bool");
        fieldTypes.append("string");
        headers.append("id");
        headers.append("Блок");
        headers.append("Обозначение");
        headers.append("Инв. номер");
        headers.append("Наименование");
        headers.append("Тип");
        headers.append("Техдокумент");
        headers.append("Дата работ");
        headers.append("Дата отчета");
        headers.append("Тип работ");
        headers.append("Трудозатраты");
        headers.append("Исполнитель");
        headers.append("Выполнено");
        headers.append("Заметки");
        break;

    case UNITS:
        setWindowTitle("Справочник: Блоки");
        dbTable = "units";
        fields.append("unit_id");
        fields.append("unit_name");
        fields.append("unit_shortname");
        fields.append("unit_subsys");
        fields.append("unit_schednum");
        fieldTypes.append("int");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        headers.append("id");
        headers.append("Наименование");
        headers.append("Сокр. Наим.");
        headers.append("Подсистема РК");
        headers.append("Номер графика");
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
    case ISSUERS:
        setWindowTitle("Справочник: Выдающие");
        dbTable = "issuers";
        fields.append("iss_id");
        fields.append("iss_name");
        fields.append("iss_loc");
        fields.append("iss_default");
        fieldTypes.append("int");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("bool");
        headers.append("id");
        headers.append("ФИО");
        headers.append("Должность");
        headers.append("По умолчанию");
        break;
    case MATERIALS:
        setWindowTitle("Справочник: Материалы");
        dbTable = "materials";
        fields.append("mat_id");
        fields.append("mat_name");
        fields.append("mat_doc");
        fields.append("mat_measure");
        fieldTypes.append("int");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("string");
        headers.append("id");
        headers.append("Материал");
        headers.append("Документ");
        headers.append("Ед. Изм.");
        break;
    case SIGNERS:
        setWindowTitle("Справочник: Подписанты");
        dbTable = "signers";
        fields.append("sig_id");
        fields.append("sig_name");
        fields.append("sig_loc");
        fields.append("sig_hidden");
        fieldTypes.append("int");
        fieldTypes.append("string");
        fieldTypes.append("string");
        fieldTypes.append("bool");
        headers.append("id");
        headers.append("ФИО");
        headers.append("Должность");
        headers.append("Скрыт");
        break;
    case VARIABLES:
        setWindowTitle("Справочник: Переменные");
        dbTable = "variables";
        fields.append("var_id");
        fields.append("var_name");
        fields.append("var_value");
        fieldTypes.append("int");
        fieldTypes.append("string");
        fieldTypes.append("string");
        headers.append("id");
        headers.append("Переменная");
        headers.append("Значение");
        break;
    }

    if (dbTable == "schedule")
        ui->copyButton->setVisible(true);
    else
        ui->copyButton->setVisible(false);
    ui->table->setColumnCount(fields.count());
    ui->table->horizontalHeader()->setFont(QFont(QFont().defaultFamily(), -1, QFont::Bold));
    ui->table->setHorizontalHeaderLabels(headers);
    if (headers[0] == "id")
        ui->table->hideColumn(0);
    ui->table->verticalHeader()->hide();
    updateData();
}

void DictionaryForm::updateData()
{
    ui->table->setSortingEnabled(false);
    while (ui->table->rowCount()>0) ui->table->removeRow(0);
    if (!db->execQuery("SELECT " + db->explodeFields(fields, 0) + " FROM " + dbTable)) {
        db->showError(this);
        return;
    }

    for (int i = 0; db->nextRecord(); i++)
    {
        ui->table->insertRow(i);
        for (int j=0; j<fields.count(); j++){
            if (fieldTypes.at(j) == "bool") {
                QTableWidgetItem *it = new QTableWidgetItem();
                it->data(Qt::CheckStateRole);
                it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                if (db->fetchValue(j).toBool())
                    it->setCheckState(Qt::Checked);
                else
                    it->setCheckState(Qt::Unchecked);
                ui->table->setItem(i, j, it);
            } else
                ui->table->setItem(i, j, new QTableWidgetItem(db->fetchValue(j).toString()));
        }
    }
    ui->table->resizeColumnsToContents();
    ui->table->setSortingEnabled(true);
    if (headers[0] == "id")
        ui->table->sortItems(1);
    else
        ui->table->sortItems(0);
    ui->table->selectRow(0);
}

void DictionaryForm::addRecord()
{
    QVariant lId;
    if (db->execQuery("INSERT INTO " + dbTable + " DEFAULT VALUES" )) {
        lId = db->lastInsertId();
       if (lId.isValid()) {
            ui->table->setSortingEnabled(false);
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
            ui->table->setSortingEnabled(true);
        }
    } else {
        db->showError(this);
}
}

void DictionaryForm::deleteRecord()
{
    int cr = ui->table->currentRow();
    if (cr<0) return;
    QString crId = ui->table->item(cr,0)->text();

    if (db->execQuery("DELETE FROM " + dbTable + " WHERE " + fields.at(0) + "=" + crId))
        ui->table->removeRow(cr);
    else
        db->showError(this);
}

void DictionaryForm::cellDoubleClicked(int row, int column)
{
    QString crId = ui->table->item(row, 0)->text();
    if (fieldTypes.at(column) == "bool") {
        bool cs = (ui->table->item(row, column)->checkState() == Qt::Checked);
        if(db->execQuery("UPDATE " + dbTable + " SET " + fields.at(column) + "=" + QVariant(!cs).toString() + " WHERE " + fields.at(0) + "=" + crId)) {
            if (cs)
                ui->table->item(row, column)->setCheckState(Qt::Unchecked);
            else
                ui->table->item(row, column)->setCheckState(Qt::Checked);
        }
        else {
            db->showError(this);
            return;
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

    if (db->execQuery("UPDATE " + dbTable + " SET " + fields.at(editor->getColumn()) + "='" +
                     editor->text() + "'WHERE " + fields.at(0) + "=" + crId)){
        ui->table->item(editor->getRow(), editor->getColumn())->setText(editor->text());
        ui->table->resizeColumnToContents(editor->getColumn());
    } else
        db->showError(this);

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
    importForm->deleteLater();
    updateData();
}

void DictionaryForm::copyRecord()
{
    QVariant lId;
    int curRow;
    QStringList toFields;
    QString query = "INSERT INTO %1 DEFAULT VALUES";
    QString prepQuery, value;
    curRow = ui->table->currentRow();
    if (curRow < 0) return;

    QMessageBox::StandardButton btn = QMessageBox::question(this, "Внимание!!!", "Вы действительно хотите скопировать запись?");
    if (btn == QMessageBox::No)
        return;

    query = query.arg(dbTable);

    db->startTransaction();

    if (!db->execQuery(query)) {
       db->showError(this);
       db->rollbackTransaction();
       return;
    }
    lId = db->lastInsertId();
    if (!lId.isValid()){
        db->rollbackTransaction();
        return;
    }
    query = "UPDATE %1 SET %2='%3' WHERE %4 = '%5'";
    query = query.arg(dbTable);
    for (int i=1; i<ui->table->columnCount(); i++)
    {
        value = ui->table->item(curRow, i)->text();
        if (i == 2) value += " копия";
        if (fieldTypes[i] == "bool") value = "FALSE";
        prepQuery = query.arg(fields[i]).arg(value).arg(fields[0]).arg(lId.toString());
        toFields.append(value);
        if (!db->execQuery(prepQuery)) {
            db->showError(this);
            db->rollbackTransaction();
            return;
        }
    }
    db->commitTransaction();
    updateData();
}
