#include "krreportpreviewform.h"
#include "ui_krreportpreviewform.h"

KRReportPreviewForm::KRReportPreviewForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::KRReportPreviewForm)
{
    ui->setupUi(this);
}

KRReportPreviewForm::~KRReportPreviewForm()
{
    delete ui;
}
void KRReportPreviewForm::setDatabase(Database *db)
{
    this->db = db;
}

QStringList KRReportPreviewForm::makeAVR(QString reportId)
{
    QStringList result;
    QString page, unitShortName, date, begDate, endDate, planBegDate, planEndDate;
    QString orderDate, orderNum, schedNum, docNum, works;
    QString ownerLoc, ownerName, member1Loc, member1Name, member2Loc, member2Name, member3Loc, member3Name;
    QString chiefLoc, chiefState;
    QString query = "SELECT krr";

    return result;
}
