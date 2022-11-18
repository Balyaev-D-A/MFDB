#include "dragdroptable.h"
#include <QDragEnterEvent>
#include <QDropEvent>


DragDropTable::DragDropTable(QWidget *parent)
    :SearchTable(parent)
{
    persistentRows.clear();
}

void DragDropTable::dragEnterEvent(QDragEnterEvent *event)
{
    if (acceptFrom == nullptr) return;
    if (event->source() == acceptFrom)
        event->acceptProposedAction();
    if (movableRows && event->source() == this)
        event->acceptProposedAction();
}

void DragDropTable::dropEvent(QDropEvent *event)
{
    int moveFrom, moveTo;
    QTableWidgetItem *itemTo;
    if (event->source() == acceptFrom)
        emit itemDroped();
    if (movableRows && event->source() == this)
    {
        moveFrom = currentRow();
        itemTo = itemAt(event->pos());
        if (itemTo == nullptr)
            moveTo = rowCount();
        else
            moveTo = itemTo->row();
        insertRow(moveTo);
        if (moveFrom > moveTo) moveFrom++;
        for (int i=0; i<columnCount(); i++)
        {
            setItem(moveTo, i, new QTableWidgetItem(*item(moveFrom, i)));
        }
        removeRow(moveFrom);
        selectRow(moveTo);
        setCurrentItem(item(moveTo,0));
        emit rowMoved();
    }
}

void DragDropTable::setAcceptFrom(QObject *obj)
{
    acceptFrom = obj;
}

void DragDropTable::setPersistentRow(int row)
{
    persistentRows.append(row);
    for (int i=0; i<columnCount(); i++)
        item(row, i)->setBackground(QBrush(QColor(Qt::yellow)));
}

bool DragDropTable::isPersistentRow(int row)
{
    return persistentRows.contains(row);
}

void DragDropTable::startDrag(Qt::DropActions actions)
{
    if (isPersistentRow(this->currentRow())) return;
    QAbstractItemView::startDrag(actions);
}

void DragDropTable::clearPersistentRows()
{
    persistentRows.clear();
}

void DragDropTable::setMovableRows(bool movable)
{
    movableRows = movable;
}
