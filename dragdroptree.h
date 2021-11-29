#ifndef DRAGDROPTREE_H
#define DRAGDROPTREE_H

#include <QTreeWidget>

class DragDropTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit DragDropTree(QWidget *parent = nullptr);
    void setAcceptFrom(QObject *obj);

private:
    QObject *acceptFrom = nullptr;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void itemDroped();
};

#endif // DRAGDROPTREE_H
