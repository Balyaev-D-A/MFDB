#include <QAbstractItemView>
#include "tablesearchwidget.h"
#include "ui_tablesearchwidget.h"

TableSearchWidget::TableSearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableSearchWidget)
{
    ui->setupUi(this);
    connect(ui->searchEdit, &QLineEdit::textEdited, this, &TableSearchWidget::searchEdited);
    connect(ui->nextButton, &QToolButton::clicked, this, &TableSearchWidget::nextClicked);
    connect(ui->prevButton, &QToolButton::clicked, this, &TableSearchWidget::prevClicked);
    ui->nextButton->setDisabled(true);
    ui->prevButton->setDisabled(true);
    defStyle = ui->searchEdit->styleSheet();
    ui->label->setStyleSheet(defStyle);
}

TableSearchWidget::~TableSearchWidget()
{
    delete ui;
}

void TableSearchWidget::setTableWidget(QTableWidget *table)
{
    tw = table;
}

void TableSearchWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    ui->searchEdit->selectAll();
    ui->searchEdit->setFocus();
}

void TableSearchWidget::searchEdited()
{
    if (ui->searchEdit->text() == "") {
        ui->searchEdit->setStyleSheet(defStyle);
        ui->nextButton->setDisabled(true);
        ui->prevButton->setDisabled(true);
        ui->label->setText("");
        return;
    }
    found = tw->findItems(ui->searchEdit->text(), Qt::MatchContains);
    if (found.count() == 0) {
        ui->searchEdit->setStyleSheet("background: rgb(255, 200, 200);");
        ui->nextButton->setDisabled(true);
        ui->prevButton->setDisabled(true);
        ui->label->setText("");
        return;
    }

    currentItem = 0;
    ui->prevButton->setDisabled(true);
    if (found.count() == 1) ui->nextButton->setDisabled(true);
    else ui->nextButton->setDisabled(false);
    ui->searchEdit->setStyleSheet(defStyle);
    tw->setCurrentItem(found[0]);
    tw->selectRow(found[0]->row());
    tw->scrollToItem(found[0], QAbstractItemView::PositionAtCenter);
    ui->label->setText(QString(" %1 из %2").arg(currentItem+1).arg(found.count()));
}

void TableSearchWidget::nextClicked()
{
    currentItem++;
    tw->setCurrentItem(found[currentItem]);
    tw->selectRow(found[currentItem]->row());
    tw->scrollToItem(found[currentItem], QAbstractItemView::PositionAtCenter);
    ui->prevButton->setDisabled(false);
    ui->label->setText(QString(" %1 из %2").arg(currentItem+1).arg(found.count()));
    if (currentItem == found.count()-1) ui->nextButton->setDisabled(true);
}

void TableSearchWidget::prevClicked()
{
    currentItem--;
    tw->setCurrentItem(found[currentItem]);
    tw->selectRow(found[currentItem]->row());
    tw->scrollToItem(found[currentItem], QAbstractItemView::PositionAtCenter);
    ui->nextButton->setDisabled(false);
    ui->label->setText(QString(" %1 из %2").arg(currentItem+1).arg(found.count()));
    if (currentItem == 0) ui->prevButton->setDisabled(true);
}

void TableSearchWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) hide();
}

void TableSearchWidget::reset()
{
    ui->searchEdit->setText("");
    ui->searchEdit->setStyleSheet(defStyle);
    ui->label->setText("");
    currentItem = -1;
    found.clear();
    ui->nextButton->setDisabled(true);
    ui->prevButton->setDisabled(true);
}
