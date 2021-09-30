#ifndef IMPORTFORM_H
#define IMPORTFORM_H

#include <QWidget>
#include <QHeaderView>
#include <QComboBox>
#include "database.h"

namespace Ui {
class ImportForm;
}

class ImportForm : public QWidget
{
    Q_OBJECT

public:
    explicit ImportForm(QWidget *parent = nullptr);
    ~ImportForm();
    void setDatabase(Database *db);
    void setHeaders(QStringList headers);
    void setFields(QStringList fields);
    void setTable(QString table);

private:
    Ui::ImportForm *ui;
    Database *db;
    QStringList headers;
    QStringList fields;
    QString dbTable;
    QComboBox hcb;
    QHeaderView *hHeader;
    int comboCurrent;

private slots:
    void fileButtonClicked();
    void readButtonClicked();
    void importButtonClicked();
    void cancelButtonClicked();
    void headerSectionClicked(int index);
    void cbTextChanged(const QString &text);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed();
};

#endif // IMPORTFORM_H
