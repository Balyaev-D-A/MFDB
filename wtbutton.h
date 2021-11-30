#ifndef WTBUTTON_H
#define WTBUTTON_H

#include <QToolButton>

class WTButton : public QToolButton
{
    Q_OBJECT
public:
    explicit WTButton(QWidget *parent = nullptr);

protected:
    void focusOutEvent(QFocusEvent *event);

signals:
    void focusLost();
};

#endif // WTBUTTON_H
