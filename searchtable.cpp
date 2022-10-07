#include "searchtable.h"

SearchTable::SearchTable(QWidget* parent)
    :QTableWidget(parent)
{
    initSearch();
}

SearchTable::SearchTable(int rows, int columns, QWidget *parent)
    :QTableWidget(rows, columns, parent)
{
    initSearch();
}

SearchTable::~SearchTable()
{
    search->deleteLater();
}
void SearchTable::initSearch()
{
    search = new TableSearchWidget(this);
    search->setTableWidget(this);
    search->move(0, 0);
    search->setGeometry(0, 0, this->width(), search->height());
}

void SearchTable::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_F) && (event->modifiers() == Qt::ControlModifier)) {
        search->show();
    }
    if (event->key() == Qt::Key_Escape) {
        search->hide();
    }
    QWidget::keyPressEvent(event);
}

void SearchTable::showEvent(QShowEvent* event)
{
    search->reset();
    search->hide();
    QWidget::showEvent(event);
}

void SearchTable::resizeEvent(QResizeEvent* event)
{
    QTableWidget::resizeEvent(event);
    search->setGeometry(0, 0, this->width(), search->height());
}
