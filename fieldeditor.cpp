#include <QDebug>
#include "fieldeditor.h"

FieldEditor::FieldEditor(QWidget *parent)
    : QLineEdit(parent)
{
    m_intvalidator = new QIntValidator(this);
    m_doublevalidator = new QDoubleValidator(this);
}

FieldEditor::~FieldEditor()
{
    delete m_intvalidator;
    delete m_doublevalidator;
}

void FieldEditor::setType(EType type)
{
    switch(type){
    case ESTRING:
        this->setValidator(NULL);
        break;
    case EINT:
        m_intvalidator->setRange(0, 999999999);
        this->setValidator(m_intvalidator);
        break;
    case EREAL:
        m_doublevalidator->setRange(0, 9999999, 3);
        this->setValidator(m_doublevalidator);
        changePeriod = true;
        break;
    case EDIGIT:
        m_intvalidator->setRange(0, 9);
        this->setValidator(m_intvalidator);
        break;
    }
}

void FieldEditor::focusOutEvent(QFocusEvent *e)
{
    emit rejectInput(this);
    e->accept();
}

void FieldEditor::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Escape:
        emit rejectInput(this);
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        emit acceptInput(this);
        break;
    case Qt::Key_Period:
        if (changePeriod) {
            insert(",");
            return;
        }
    }

    QLineEdit::keyPressEvent(e);
}

void FieldEditor::setCell(int row, int column)
{
    m_row = row;
    m_col = column;
}

int FieldEditor::getRow()
{
    return m_row;
}

int FieldEditor::getColumn()
{
    return m_col;
}
