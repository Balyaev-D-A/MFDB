#include "defectstable.h"
#include <QKeyEvent>

DefectsTable::DefectsTable(QWidget *parent)
    :QTableWidget(parent)
{

}

void DefectsTable::keyPressEvent(QKeyEvent *event)
{
    QTableWidget::keyPressEvent(event);
    if (event->key()  == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        emit clearCellPressed();
    }
}
