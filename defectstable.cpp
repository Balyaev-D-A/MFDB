#include "defectstable.h"
#include <QKeyEvent>

DefectsTable::DefectsTable(QWidget *parent)
    :SearchTable(parent)
{

}

void DefectsTable::keyPressEvent(QKeyEvent *event)
{
    SearchTable::keyPressEvent(event);
    if (event->key()  == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        emit clearCellPressed();
    }
}
