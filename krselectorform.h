#ifndef KRSELECTORFORM_H
#define KRSELECTORFORM_H

#include <QWidget>

#include "database.h"

namespace Ui {
class KRSelectorForm;
}

typedef struct {
    QString sched;
    QString device;
    QString type;
    QString kks;
} KRDevice;

class KRSelectorForm : public QWidget
{
    Q_OBJECT

public:
    explicit KRSelectorForm(QWidget *parent = nullptr);
    ~KRSelectorForm();
    void setDatabase(Database *db);

private:
    Ui::KRSelectorForm *ui;
    Database *db;

private slots:
    void cancelClicked();
    void okClicked();
    void updateTable();

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void closed(KRSelectorForm *sender);
    void selected(const KRDevice &device);
};

#endif // KRSELECTORFORM_H
