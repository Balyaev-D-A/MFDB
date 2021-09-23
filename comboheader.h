#ifndef COMBOHEADER_H
#define COMBOHEADER_H

#include <QHeaderView>

class ComboHeader : public QHeaderView
{
    Q_OBJECT
public:
    ComboHeader(Qt::Orientation orientation, QWidget *parent);
};

#endif // COMBOHEADER_H
