#ifndef VERIFYFORM_H
#define VERIFYFORM_H

#include <QWidget>

#include "database.h"

namespace Ui {
class VerifyForm;
}

class VerifyForm : public QWidget
{
    Q_OBJECT

public:
    explicit VerifyForm(QWidget *parent = nullptr);
    ~VerifyForm();
    void setDatabase(Database *db);

private:
    Ui::VerifyForm *ui;
    Database *db;

protected:
    void showEvent(QShowEvent *event);
};

#endif // VERIFYFORM_H
