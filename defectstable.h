#ifndef DEFECTSTABLE_H
#define DEFECTSTABLE_H

#include <QWidget>
#include <QTableWidget>
#include "searchtable.h"

class DefectsTable : public SearchTable
{
    Q_OBJECT
public:
    explicit DefectsTable(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event);
signals:
    void clearCellPressed();
};

#endif // DEFECTSTABLE_H
