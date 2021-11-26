#include "dragdroptable.h"
#include <QApplication>

DragDropTable::DragDropTable()
{

}

DragDropTable::~DragDropTable()
{

}

void DragDropTable::mousePressEvent(QMouseEvent *event)
{
    dragStart = event->pos();
}

void DragDropTable::mouseMoveEvent(QMouseEvent *event)
{
    int dist = (event->pos() - dragStart).manhattanLength();

    if(dist >= QApplication::startDragDistance() && (event->buttons() & Qt::LeftButton)) {

    }
}
