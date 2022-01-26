#ifndef DEVICESELECTORFORM_H
#define DEVICESELECTORFORM_H

#include <QWidget>
#include "database.h"

typedef struct {
    QString devType;
    QString devName;
    QString devKKS;
} SelectedDevice;

class QTreeWidgetItem;

namespace Ui {
class DeviceSelectorForm;
}

class DeviceSelectorForm : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceSelectorForm(QWidget *parent = nullptr);
    ~DeviceSelectorForm();
    void setDatabase(Database *db);

private:
    Ui::DeviceSelectorForm *ui;
    Database *db;
    void fillDeviceBox();

private slots:
    void updateDeviceTree();
    void dtItemDoubleClicked(QTreeWidgetItem *item, int column);
    void okClicked();
    void cancelClicked();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
signals:
    void closed(DeviceSelectorForm *sender);
    void deviceChoosed(SelectedDevice device);
};

#endif // DEVICESELECTORFORM_H
