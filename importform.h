#ifndef IMPORTFORM_H
#define IMPORTFORM_H

#include <QWidget>
#include <QComboBox>
#include <QHeaderView>
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
    QComboBox *hcb;
    QStringList usedHeaders;
    QHeaderView *hHeader;

private slots:
    void fileButtonClicked();
    void readButtonClicked();
    void importButtonClicked();
    void cancelButtonClicked();
    void headerSectionClicked(int index);
    void cbTextChoosed(QString text);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed();
};

#endif // IMPORTFORM_H
