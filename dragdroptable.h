#ifndef DRAGDROPTABLE_H
#define DRAGDROPTABLE_H

#include <QTableWidget>
#include <QMouseEvent>

class DragDropTable : public QTableWidget
{
    Q_OBJECT
public:
    DragDropTable();
    ~DragDropTable();
private:
    QPoint dragStart;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

};

#endif // DRAGDROPTABLE_H
