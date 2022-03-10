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

QString KRReportPreviewForm::minDate(QString date1, QString date2)
{
    return date1;
}

QString KRReportPreviewForm::maxDate(QString date1, QString date2)
{
    return date1;
}

QStringList KRReportPreviewForm::makeAVR(QString reportId)
{
    typedef struct {
        QString devType;
        QString devKSS;
        QString devName;
        QString begDate;
        QString endDate;
        QString workHours;
        QString workDesc;
    } ReportWork;

    QList<ReportWork> ReportWorks;
    ReportWork rw;
    QStringList result;
    QStringList works;
    QString page, unitShortName, subsystem,date, begDate, endDate, planBegDate, planEndDate;
    QString orderDate, orderNum, schedNum, docNum, workType;
    QString ownerLoc, ownerName, member1Loc, member1Name, member2Loc, member2Name, member3Loc, member3Name;
    QString repairerLoc, repairerName, chiefLoc, chiefName;
    QString query = "SELECT unit_shortname, unit_subsys, krr_planbeg, krr_planend, krr_date, krr_docnum FROM krreports"
                    "LEFT JOIN units ON krr_unit = unit_id "
                    "WHERE krr_id = '%1'";
    query = query.arg(reportId);

    if (db->execQuery(query)) {
        if (db->nextRecord()) {
            unitShortName = db->fetchValue(0).toString();
            subsystem = db->fetchValue(1).toString();
            planBegDate = db->fetchValue(2).toString();
            planEndDate = db->fetchValue(3).toString();
            date = db->fetchValue(4).toString();
            docNum = db->fetchValue(5).toString();
        }
    } else {
        db->showError(this);
        return result;
    }

    query = "SELECT sch_name, sch_type, sch_kks, kr_begdate, kr_enddate FROM krrworks "
            "LEFT JOIN kaprepairs ON krw_work = kr_id "
            "LEFT JOIN schedule ON kr_sched = sch_id "
            "WHERE krr_id = '%1'";
    query = query.arg(reportId);
    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    begDate = "99.99.9999";
    endDate = "00.00.0000";
    while (db->nextRecord())
    {
        works.append(db->fetchValue(0).toString() + " " + db->fetchValue(1).toString() + " " + db->fetchValue(2).toString());
        rw.devName = db->fetchValue(0).toString();
        rw.devType = db->fetchValue(1).toString();
        rw.devKSS = db->fetchValue(2).toString();
        rw.begDate = db->fetchValue(3).toString();
        rw.endDate = db->fetchValue(4).toString();
        begDate = minDate(begDate, db->fetchValue(1).toString());
        endDate = maxDate(endDate, db->fetchValue(2).toString());
    }

    query = "SELECT sig_loc, sig_name, krs_role FROM krsigners "
            "LEFT JOIN signers ON krs_signer = sig_id "
            "WHERE krs_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    while (db->nextRecord())
    {
        switch (db->fetchValue(2).toInt()) {
        case OWNER:
            ownerLoc = db->fetchValue(0).toString();
            ownerName = db->fetchValue(1).toString();
            break;
        case MEMBER1:
            member1Loc = db->fetchValue(0).toString();
            member1Name = db->fetchValue(1).toString();
            break;
        case MEMBER2:
            member2Loc = db->fetchValue(0).toString();
            member2Name = db->fetchValue(1).toString();
            break;
        case MEMBER3:
            member3Loc = db->fetchValue(0).toString();
            member3Name = db->fetchValue(1).toString();
            break;
        case REPAIRER:
            repairerLoc = db->fetchValue(0).toString();
            repairerName = db->fetchValue(1).toString();
            break;
        case CHIEF:
            chiefLoc = db->fetchValue(0).toString();
            chiefName = db->fetchValue(1).toString();
            break;
        }
    }

    orderDate = db->getVariable("ДатаПриказа").toString();
    orderNum = db->getVariable("ПриказПоАЭС").toString();
    workType = "КР";

    return result;
}

