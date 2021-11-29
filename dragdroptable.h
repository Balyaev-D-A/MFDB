#ifndef DRAGDROPTABLE_H
#define DRAGDROPTABLE_H

#include <QTableWidget>
#include <QMouseEvent>

class DragDropTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit DragDropTable(QWidget *parent = nullptr);
    void setAcceptFrom(QObject *obj);
private:
    QObject *acceptFrom = nullptr;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void itemDroped();
};

#endif // DRAGDROPTABLE_H