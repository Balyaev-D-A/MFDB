#include "defectform.h"
#include "ui_defectform.h"

DefectForm::DefectForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DefectForm)
{
    ui->setupUi(this);
    connect(ui->deviceButton, &QToolButton::clicked, this, &DefectForm::deviceButtonClicked);
}

DefectForm::~DefectForm()
{
    delete ui;
}

void DefectForm::setDatabase(Database *db)
{
    this->db = db;
}

void DefectForm::deviceButtonClicked()
{
    dsf = new DeviceSelectorForm();
    dsf->setDatabase(db);
    connect(dsf, &DeviceSelectorForm::closed, this, &DefectForm::selectorClosed);
    connect(dsf, &DeviceSelectorForm::deviceChoosed, this, &DefectForm::deviceChoosed);
    dsf->show();
}

void DefectForm::selectorClosed(DeviceSelectorForm *sender)
{
    sender->deleteLater();
}

void DefectForm::deviceChoosed(SelectedDevice device)
{
    this->device = device;
    ui->deviceEdit->setText(device.devName + " " + device.devType + " " + device.devKKS);
    updateDefects();
    updateRepairs();
}

void DefectForm::updateDefects()
{
    Defect defect;
    QString query = "SELECT drd_id, drd_stage, drd_desc FROM defrealdescs WHERE drd_dev = '%1'";
    query = query.arg(device.devType);
    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    defectList.clear();

    while (db->nextRecord())
    {
        defect.id = db->fetchValue(0).toInt();
        defect.stage = static_cast<DefectStage>(db->fetchValue(1).toInt());
        defect.description = db->fetchValue(2).toString();
        defectList.append(defect);
    }
}
