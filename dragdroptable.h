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
    void setPersistentRow(int row);
    bool isPersistentRow(int row);
private:
    QObject *acceptFrom = nullptr;
    QList<int> persistentRows;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void startDrag(Qt::DropActions actions);

signals:
    void itemDroped();
};

#endif // DRAGDROPTABLE_H
