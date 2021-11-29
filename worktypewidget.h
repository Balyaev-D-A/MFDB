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
};

#endif // WORKTYPEWIDGET_H
