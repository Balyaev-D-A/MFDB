#ifndef CONNECTIONFORM_H
#define CONNECTIONFORM_H

#include <QWidget>
#include "settings.h"

namespace Ui {
class ConnectionForm;
}

class ConnectionForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionForm(QWidget *parent = nullptr);
    ~ConnectionForm();
signals:
    void connectClicked(Settings::ConnSettings cs);

private slots:
    void on_cancelButton_clicked();
    void on_connectButton_clicked();

private:
    Ui::ConnectionForm *ui;
};

#endif // CONNECTIONFORM_H
