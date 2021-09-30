#ifndef HEADERCOMBOBOX_H
#define HEADERCOMBOBOX_H

#include <QComboBox>

class HeaderComboBox : public QComboBox
{
    Q_OBJECT
public:
    HeaderComboBox(QWidget *parent = nullptr);

protected:
    void focusOutEvent(QFocusEvent *e);

signals:
    void focusOut();
};

#endif // HEADERCOMBOBOX_H
