#ifndef RECEIPTFORM_H
#define RECEIPTFORM_H

#include <QWidget>

#include "database.h"

namespace Ui {
class ReceiptForm;
}

class ReceiptForm : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptForm(QWidget *parent = nullptr);
    ~ReceiptForm();
    void setDatabase(Database *db);
    void newReceipt();
    void editReceipt(QString recId);

private:
    Ui::ReceiptForm *ui;
    Database *db;
    QString recId;
    bool matsChanged;
    void updateMaterials();
    bool saveReceipt();

private slots:
    void addMaterialClicked();
    void removeMaterialClicked();
    void okClicked();
    void cancelClicked();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void receiptSaved();
    void closed(ReceiptForm *sender);
};

#endif // RECEIPTFORM_H
