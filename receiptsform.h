#ifndef RECEIPTSFORM_H
#define RECEIPTSFORM_H

#include <QWidget>

#include "database.h"
#include "receiptform.h"

namespace Ui {
class ReceiptsForm;
}

class ReceiptsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiptsForm(QWidget *parent = nullptr);
    ~ReceiptsForm();
    void setDatabase(Database *db);

private:
    Ui::ReceiptsForm *ui;
    Database *db;

private slots:
    void updateTable();
    void addReceiptClicked();
    void editReceiptClicked();
    void deleteReceiptClicked();
    void receiptFormClosed(ReceiptForm *sender);

protected:
    void showEvent(QShowEvent *event);
};

#endif // RECEIPTSFORM_H
