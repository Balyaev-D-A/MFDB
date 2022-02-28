#include <QDragEnterEvent>
#include "dragdropedit.h"

DragDropEdit::DragDropEdit(QWidget *parent)
            :QLineEdit(parent)
{

}

void DragDropEdit::setAcceptFrom(QObject *obj)
{
    acceptFrom = obj;
}

void DragDropEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (acceptFrom == nullptr) return;
    if (event->source() == acceptFrom)
        event->acceptProposedAction();
}

void DragDropEdit::dropEvent(QDropEvent *event)
{
    if (event->source() == acceptFrom)
        emit itemDroped();
}
