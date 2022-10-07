#ifndef TABLESEARCHWIDGET_H
#define TABLESEARCHWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QShowEvent>

namespace Ui {
class TableSearchWidget;
}

class TableSearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableSearchWidget(QWidget *parent = nullptr);
    ~TableSearchWidget();
    void setTableWidget(QTableWidget* table);
public slots:
    void reset();

protected:
    void showEvent(QShowEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::TableSearchWidget *ui;
    QTableWidget* tw;
    QList<QTableWidgetItem*> found;
    QString defStyle;
    int currentItem = -1;

private slots:
    void searchEdited();
    void nextClicked();
    void prevClicked();
};

#endif // TABLESEARCHWIDGET_H
