#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include "database.h"

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = nullptr);
    ~ImportDialog();
    void setDatabase(Database *db);
    void setHeaders(QStringList headers);
    void setFields(QStringList fields);
    void setTable(QString table);

private:
    Ui::ImportDialog *ui;
    Database *db;
    QStringList headers;
    QStringList fields;
    QString dbTable;

private slots:
    void fileButtonClicked();
    void readButtonClicked();
    void importButtonClicked();
    void cancelButtonClicked();
};

#endif // IMPORTDIALOG_H
