#ifndef DICTIONARYFORM_H
#define DICTIONARYFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QIntValidator>

#include "database.h"
#include "fieldeditor.h"
#include "importdialog.h"

enum Dictionary {
    EMPLOYEES,
    UNITS,
    SCHEDULE,
    LOCATIONS,
};


namespace Ui {
class DictionaryForm;
}

class DictionaryForm : public QWidget
{
    Q_OBJECT

public:
    explicit DictionaryForm(QWidget *parent = nullptr);
    ~DictionaryForm();
    void setDatabase(Database *db);
    void setDictionary(Dictionary dictionary);


private slots:
    void addRecord();
    void deleteRecord();
    void cellDoubleClicked(int row, int column);
    void inputAccepted();
    void inputRejected();
    void updateData();
    void importBtnClicked();
    void importDlgFinished(int result);

private:
    QString explodeFields(unsigned char from);
    Ui::DictionaryForm *ui;
    Dictionary dict;
    Database *db;
    QString dbTable;
    QStringList fields;
    QStringList fieldTypes;
    QStringList headers;
    unsigned char sortingColumn;
    Qt::SortOrder sortingOrder;
    FieldEditor *editor;
    ImportDialog *importDlg;
};

#endif // DICTIONARYFORM_H
