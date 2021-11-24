#ifndef RASPFORM_H
#define RASPFORM_H

#include <QWidget>
#include <QMap>
#include "database.h"

struct EmpInfo
{
    int id;
    bool metrolog;
};

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
    QMap<QString, EmpInfo> empmap;
    bool hasMA = true;
    void updateTotal();
    void updateRaspTotal();
private slots:
    void updateWorkTable();
    void updateMembers();
    void addMemberClicked();
    void removeMemberClicked();
    void addWorkClicked();
//    void removeWorkPressed();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed(RaspForm *sender);
};

#endif // RASPFORM_H
