#ifndef SEARCHTABLE_H
#define SEARCHTABLE_H

#include <QTableWidget>
#include <QWidget>
#include "tablesearchwidget.h"

class SearchTable : public QTableWidget
{
    Q_OBJECT
public:
    SearchTable(QWidget *parent = nullptr);
    SearchTable(int rows, int columns, QWidget *parent = nullptr);
    ~SearchTable();

protected:
    void keyPressEvent(QKeyEvent* event);
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);
private:
    void initSearch();
    TableSearchWidget* search;


};

#endif // SEARCHTABLE_H
