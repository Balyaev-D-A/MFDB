#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QShowEvent>
#include "settings.h"

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsForm(QWidget *parent = nullptr);
    ~SettingsForm();

private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();

signals:
    void saved();

private:
    Ui::SettingsForm *ui;
    Settings *settings;

protected:
    void showEvent(QShowEvent *event);
};

#endif // SETTINGSFORM_H
