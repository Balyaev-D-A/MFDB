#ifndef WORKTYPEWIDGET_H
#define WORKTYPEWIDGET_H

#include <QWidget>

namespace Ui {
class WorkTypeWidget;
}

class WorkTypeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WorkTypeWidget(QWidget *parent = nullptr);
    ~WorkTypeWidget();
    void setWorkTypes(QString wt);
    QString workTypes();

private:
    Ui::WorkTypeWidget *ui;

private slots:
    void checkBoxStateChanged();
    void okButtonClicked();

protected:
    void focusOutEvent(QFocusEvent *event);

signals:
    void typeChanged();
    void widgetHidden(QWidget *sender);
};

#endif // WORKTYPEWIDGET_H
