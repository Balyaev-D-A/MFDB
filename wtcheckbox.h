#ifndef WTCHECKBOX_H
#define WTCHECKBOX_H

#include <QCheckBox>

class WTCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    explicit WTCheckBox(QWidget *parent = nullptr);

protected:
    void focusOutEvent(QFocusEvent *event);

signals:
    void focusLost();
};

#endif // WTCHECKBOX_H
