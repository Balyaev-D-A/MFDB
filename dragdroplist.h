#ifndef DRAGDROPLIST_H
#define DRAGDROPLIST_H

#include <QListWidget>

class DragDropList : public QListWidget
{
    Q_OBJECT
public:
    explicit DragDropList(QWidget *parent = nullptr);
    void setAcceptFrom(QObject *obj);

private:
    QObject *acceptFrom = nullptr;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void itemDroped();
};

#endif // DRAGDROPLIST_H
