#include "dragdroptable.h"
#include <QDragEnterEvent>
#include <QDropEvent>


DragDropTable::DragDropTable(QWidget *parent)
    :QTableWidget(parent)
{
    persistentRows.clear();
}

void DragDropTable::dragEnterEvent(QDragEnterEvent *event)
{
    if (acceptFrom == nullptr) return;
    if (event->source() == acceptFrom)
        event->acceptProposedAction();
}

void DragDropTable::dropEvent(QDropEvent *event)
{
    if (event->source() == acceptFrom)
        emit itemDroped();
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
