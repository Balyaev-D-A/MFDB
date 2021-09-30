#include "headercombobox.h"

HeaderComboBox::HeaderComboBox(QWidget *parent) :
    QComboBox(parent)
{

}

void HeaderComboBox::focusOutEvent(QFocusEvent *e)
{
    QComboBox::focusOutEvent(e);
    emit focusOut();
}
