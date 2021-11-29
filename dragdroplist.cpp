#include "dragdroplist.h"
#include <QDragEnterEvent>
#include <QDropEvent>

DragDropList::DragDropList(QWidget *parent)
    :QListWidget(parent)
{

}

void DragDropList::setAcceptFrom(QObject *obj)
{
    acceptFrom = obj;
}

void DragDropList::dragEnterEvent(QDragEnterEvent *event)
{
    if (acceptFrom == nullptr) return;
    if (event->source() == acceptFrom)
        event->acceptProposedAction();
}

void DragDropList::dropEvent(QDropEvent *event)
{
    if (event->source() == acceptFrom)
        emit itemDroped();
}
