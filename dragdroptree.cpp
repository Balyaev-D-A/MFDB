#include "dragdroptree.h"
#include <QDragEnterEvent>
#include <QDropEvent>

DragDropTree::DragDropTree(QWidget *parent)
    :QTreeWidget(parent)
{

}

void DragDropTree::setAcceptFrom(QObject *obj)
{
    acceptFrom = obj;
}

void DragDropTree::dragEnterEvent(QDragEnterEvent *event)
{
    if (acceptFrom == nullptr) return;
    if (event->source() == acceptFrom)
        event->acceptProposedAction();
}

void DragDropTree::dropEvent(QDropEvent *event)
{
    if (event->source() == acceptFrom)
        emit itemDroped();
}
