#include "dragdroptable.h"
#include <QApplication>
#include <QDrag>
#include <QMimeData>

DragDropTable::DragDropTable(QWidget *parent)
    :QTableWidget(parent)
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
    QString value;
    QString data;

    int dist = (event->pos() - dragStart).manhattanLength();

    if(dist >= QApplication::startDragDistance() && (event->buttons() & Qt::LeftButton)) {
        QDrag *drag = new QDrag(this);
        QMimeData *mime = new QMimeData();


    }
}
