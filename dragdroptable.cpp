#include "dragdroptable.h"
#include <QDragEnterEvent>
#include <QDropEvent>


DragDropTable::DragDropTable(QWidget *parent)
    :QTableWidget(parent)
{

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
