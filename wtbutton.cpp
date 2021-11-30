#include "wtbutton.h"
#include <QFocusEvent>

WTButton::WTButton(QWidget *parent)
    :QToolButton(parent)
{

}

void WTButton::focusOutEvent(QFocusEvent *event)
{
    QToolButton::focusOutEvent(event);
    emit focusLost();
}
