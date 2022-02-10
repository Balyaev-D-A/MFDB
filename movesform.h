#ifndef MOVESFORM_H
#define MOVESFORM_H

#include <QWidget>

#include "database.h"

namespace Ui {
class MovesForm;
}

class MovesForm : public QWidget
{
    Q_OBJECT

public:
    explicit MovesForm(QWidget *parent = nullptr);
    ~MovesForm();
    void setDatabase(Database *db);

private:
    Ui::MovesForm *ui;
    Database *db;
    void updateMaterials();

private slots:
    void updateMoves();

protected:
    void showEvent(QShowEvent *event);
};

#endif // MOVESFORM_H
