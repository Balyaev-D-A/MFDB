#include "wtcheckbox.h"
#include <QFocusEvent>

WTCheckBox::WTCheckBox(QWidget *parent)
    :QCheckBox(parent)
{

}

void WTCheckBox::focusOutEvent(QFocusEvent *event)
{
    QCheckBox::focusOutEvent(event);
    emit focusLost();
}
