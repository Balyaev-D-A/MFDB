#ifndef RASPFORM_H
#define RASPFORM_H

#include <QWidget>
#include <QMap>
#include <QDate>
#include "database.h"
#include "fieldeditor.h"
#include "worktypewidget.h"

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
    bool editRasp(QString raspId);
    void newRasp(QDate date = QDate::currentDate());



private:
    Ui::RaspForm *ui;
    Database *db;
    QMap<QString, EmpInfo> empmap;
    bool hasMA = false;
    int lastUnitIndex;
    int currentRasp = -1;
    FieldEditor *wEditor;
    FieldEditor *cwEditor;
    WorkTypeWidget *wtWidget;
    bool worksChanged;
    bool teamChanged;
    void updateTotal();
    void updateRaspTotal();
    bool memberAdded(QString member);
    void checkMA();
private slots:
    void updateWorkTable();
    void updateMembers();
    void addMemberClicked();
    void removeMemberClicked();
    void addWorkClicked();
    void removeWorkClicked();
    void unitChanged(const QString &text);
    void wCellDblClicked(int row, int column);
    void cwCellDblClicked(int row, int column);
    void wInputAccepted();
    void wInputRejected();
    void cwInputAccepted();
    void cwInputRejected();
    void workTypesChanged();
    void checkAddedMembers();
    void saveButtonClicked();
    void cancelButtonClicked();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed(RaspForm *sender);
};

#endif // RASPFORM_H
