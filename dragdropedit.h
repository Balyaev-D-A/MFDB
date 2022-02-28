#ifndef DRAGDROPEDIT_H
#define DRAGDROPEDIT_H

#include <QLineEdit>
#include <QObject>

class DragDropEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit DragDropEdit(QWidget *parent = nullptr);
    void setAcceptFrom(QObject *obj);

private:
    QObject *acceptFrom = nullptr;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void itemDroped();
};

#endif // DRAGDROPEDIT_H
