#ifndef COMBOHEADER_H
#define COMBOHEADER_H

#include <QHeaderView>
#include <QComboBox>

class ComboHeader : public QHeaderView
{
    Q_OBJECT
public:
    ComboHeader(Qt::Orientation orientation, QWidget *parent);
    void setHeaders(QStringList headers);

private:
    struct Margins {
        int left;
        int right;
        int top;
        int bottom;
    } margins;
    QList<QComboBox*> comboList;
    QStringList headers;

protected:
//    void showEvent(QShowEvent *e);

private slots:
    void onSectionCountChanged(int oCount, int nCount);
    void onSectionResized(int lIndex, int oSize, int nSize);


};

#endif // COMBOHEADER_H
