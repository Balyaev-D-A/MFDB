#include "deviceselectorform.h"
#include "ui_deviceselectorform.h"
#include "stringlistvalidator.h"
#include <QMessageBox>
#include <QCompleter>

DeviceSelectorForm::DeviceSelectorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceSelectorForm)
{
    setWindowFlag(Qt::Window, true);
    ui->setupUi(this);
    connect(ui->deviceBox, &QComboBox::currentTextChanged, this, &DeviceSelectorForm::updateDeviceTree);
    connect(ui->okButton, &QPushButton::clicked, this, &DeviceSelectorForm::okClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &DeviceSelectorForm::cancelClicked);
    connect(ui->deviceTree, &QTreeWidget::itemDoubleClicked, this, &DeviceSelectorForm::dtItemDoubleClicked);
}

DeviceSelectorForm::~DeviceSelectorForm()
{
    delete ui;
}

void DeviceSelectorForm::setDatabase(Database *db)
{
    this->db = db;
}

void DeviceSelectorForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    fillDeviceBox();
    updateDeviceTree();
}

void DeviceSelectorForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed(this);
}

void DeviceSelectorForm::fillDeviceBox()
{
    QStringList validList;
    QString query = "SELECT DISTINCT ON (sch_type) sch_type FROM schedule WHERE sch_executor = 'ИТЦРК' ORDER BY sch_type ASC";

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    ui->deviceBox->blockSignals(true);
    while (db->nextRecord())
    {
        ui->deviceBox->addItem(db->fetchValue(0).toString());
        validList.append(db->fetchValue(0).toString());
    }
    ui->deviceBox->blockSignals(false);
    ui->deviceBox->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    ui->deviceBox->completer()->setCompletionMode(QCompleter::PopupCompletion);
    StringListValidator *validator = new StringListValidator(ui->deviceBox);
    validator->setList(validList);
    ui->deviceBox->setValidator(validator);
    ui->deviceBox->setCurrentIndex(-1);
}

void DeviceSelectorForm::updateDeviceTree()
{
    QTreeWidgetItem *unitItem, *kksItem;
    QStringList usedKKS;

    QString query = "SELECT DISTINCT ON (def_kks) def_kks FROM defects WHERE def_devtype = '%1' ORDER BY def_kks";
    query = query.arg(ui->deviceBox->currentText());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    while (db->nextRecord()) usedKKS.append(db->fetchValue(0).toString());

    query = "SELECT DISTINCT ON (sch_kks) unit_name, sch_kks FROM schedule AS s LEFT JOIN units AS u ON s.sch_unit = u.unit_id "
                    "WHERE sch_type = '%1' AND sch_executor = 'ИТЦРК' ORDER BY sch_kks";
    query = query.arg(ui->deviceBox->currentText());
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    ui->deviceTree->clear();

    while (db->nextRecord())
    {
        unitItem = nullptr;
        for (int i=0; i<ui->deviceTree->topLevelItemCount(); i++)
        {
            if (ui->deviceTree->topLevelItem(i)->text(0) == db->fetchValue(0).toString()) {
                unitItem = ui->deviceTree->topLevelItem(i);
                break;
            }
        }

        if (unitItem == nullptr) {
            unitItem = new QTreeWidgetItem();
            unitItem->setText(0, db->fetchValue(0).toString());
            ui->deviceTree->addTopLevelItem(unitItem);
        }

        kksItem = new QTreeWidgetItem();
        kksItem->setText(0, db->fetchValue(1).toString());
        if (usedKKS.contains(db->fetchValue(1).toString())) kksItem->setBackground(0, QBrush(Qt::red));
        unitItem->addChild(kksItem);
    }

    query = "SELECT sch_name FROM schedule WHERE sch_type = '%1' LIMIT 1";
    query = query.arg(ui->deviceBox->currentText().simplified());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()) {
        ui->deviceTree->setHeaderLabel(db->fetchValue(0).toString());
    }
    else {
        ui->deviceTree->setHeaderLabel("");
    }

    ui->deviceTree->sortItems(0, Qt::AscendingOrder);
}

void DeviceSelectorForm::okClicked()
{
    QTreeWidgetItem* item = ui->deviceTree->currentItem();
    bool isTopLevelItem = false;
    SelectedDevice dev;

    if (!item) {
        QMessageBox::critical(this, "Ошибка!", "Устройство не выбрано. Выберите устройство.");
        return;
    }

    for (int i=0; i<ui->deviceTree->topLevelItemCount(); i++)
    {
        if (ui->deviceTree->topLevelItem(i) == item) {
            isTopLevelItem = true;
            break;
        }
    }

    if (isTopLevelItem) {
//        QMessageBox::critical(this, "Ошибка!", "Выбран каталог устройств. Выберите отдельное устройство.");
        ui->deviceTree->expandItem(item);
        return;
    }

    dev.kks = item->text(0);
    dev.type = ui->deviceBox->currentText();
    QString query = "SELECT sch_unit, sch_name FROM schedule WHERE sch_kks = '%1' AND sch_type = '%2' LIMIT 1";
    query = query.arg(dev.kks).arg(dev.type);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    if (db->nextRecord()){
        dev.unitId = db->fetchValue(0).toString();
        dev.name = db->fetchValue(1).toString();
    }

    emit deviceChoosed(dev);
    close();
}

void DeviceSelectorForm::dtItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    okClicked();
}

void DeviceSelectorForm::cancelClicked()
{
    close();
}

void DeviceSelectorForm::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Enter) | (event->key() == Qt::Key_Return)) okClicked();
    QWidget::keyPressEvent(event);
}
