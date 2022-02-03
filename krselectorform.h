#ifndef KRSELECTORFORM_H
#define KRSELECTORFORM_H

#include <QWidget>

#include "database.h"

namespace Ui {
class KRSelectorForm;
}

typedef struct {
    QString selSched;
    QString selDevice;
    QString selType;
    QString selKKS;
} SelectedDevice;

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
    void updateTable();

private slots:
    void cancelClicked();
    void okClicked();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void closed(KRSelectorForm *sender);
    void selected(const SelectedDevice &device);
};

#endif // KRSELECTORFORM_H
