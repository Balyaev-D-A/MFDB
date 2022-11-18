#ifndef DRAGDROPTABLE_H
#define DRAGDROPTABLE_H

#include "searchtable.h"
#include <QMouseEvent>

class DragDropTable : public SearchTable
{
    Q_OBJECT
public:
    explicit DragDropTable(QWidget *parent = nullptr);
    void setAcceptFrom(QObject *obj);
    void setPersistentRow(int row);
    bool isPersistentRow(int row);
    void clearPersistentRows();
    void setMovableRows(bool movable);
private:
    QObject *acceptFrom = nullptr;
    bool movableRows = false;
    QList<int> persistentRows;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void startDrag(Qt::DropActions actions);

signals:
    void itemDroped();
    void rowMoved();
};

#endif // DRAGDROPTABLE_H
