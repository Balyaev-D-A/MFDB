#ifndef RASPFORM_H
#define RASPFORM_H

#include <QWidget>
#include "database.h"

namespace Ui {
class RaspForm;
}

class RaspForm : public QWidget
{
    Q_OBJECT

public:
    explicit RaspForm(QWidget *parent = nullptr);
    ~RaspForm();
    void setDatabase(Database *db);
//    void editRasp(int raspId);
    void newRasp();



private:
    Ui::RaspForm *ui;
    Database *db;
    void updateTotal();
    void updateRaspTotal();
private slots:
    void updateWorkTable();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed(RaspForm *sender);
};

#endif // RASPFORM_H
