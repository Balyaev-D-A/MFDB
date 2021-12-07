#ifndef FIELDEDITOR_H
#define FIELDEDITOR_H

#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QFocusEvent>
#include <QKeyEvent>

enum EType {
    ESTRING,
    EINT,
    EREAL,
    EDIGIT
};

class FieldEditor : public QLineEdit
{
    Q_OBJECT
public:
    explicit FieldEditor(QWidget *parent = nullptr);
    ~FieldEditor();
    void setType(EType type);
    void setCell(int row, int column);
    int getRow();
    int getColumn();
private:
    QIntValidator *m_intvalidator;
    QDoubleValidator *m_doublevalidator;
    int m_row;
    int m_col;

protected:
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);

signals:
    void acceptInput(FieldEditor *sender);
    void rejectInput(FieldEditor *sender);
};

#endif // FIELDEDITOR_H
