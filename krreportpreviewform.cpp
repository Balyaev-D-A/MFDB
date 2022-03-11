#include "krreportpreviewform.h"
#include "ui_krreportpreviewform.h"
#include <QFile>
#include <QMessageBox>

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
    QStringList dateList1, dateList2;

    dateList1 = date1.split(".");
    dateList2 = date2.split(".");

    if (dateList1[2].toInt() < dateList1[2].toInt()) return date1;
    else if (dateList2[2].toInt() < dateList1[2].toInt()) return date2;
    else if (dateList1[1].toInt() < dateList2[1].toInt()) return date1;
    else if (dateList2[1].toInt() < dateList1[1].toInt()) return date2;
    else if (dateList1[0].toInt() < dateList2[0].toInt()) return date1;
    else if (dateList2[0].toInt() < dateList1[0].toInt()) return date2;
    return date1;
}

QString KRReportPreviewForm::maxDate(QString date1, QString date2)
{
    QStringList dateList1, dateList2;

    dateList1 = date1.split(".");
    dateList2 = date2.split(".");

    if (dateList1[2].toInt() > dateList1[2].toInt()) return date1;
    else if (dateList2[2].toInt() > dateList1[2].toInt()) return date2;
    else if (dateList1[1].toInt() > dateList2[1].toInt()) return date1;
    else if (dateList2[1].toInt() > dateList1[1].toInt()) return date2;
    else if (dateList1[0].toInt() > dateList2[0].toInt()) return date1;
    else if (dateList2[0].toInt() > dateList1[0].toInt()) return date2;
    return date1;
}

QStringList KRReportPreviewForm::makeAVR(QString reportId)
{
    QStringList result;
    QStringList workList;
    QString page, unitShortName, subsystem,date, begDate, endDate, planBegDate, planEndDate;
    QString orderDate, orderNum, schedNum, docNum, workType, firstWork, works, accordingDoc;
    QString ownerLoc, ownerName, member1Loc, member1Name, member2Loc, member2Name, member3Loc, member3Name;
    QString repairerLoc, repairerName, chiefLoc, chiefName;
    QFile file;
    QTextStream *ts;
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

    query = "SELECT CONCAT_WS(' ', sch_name, sch_type, sch_kks), kr_begdate, kr_enddate FROM krrworks "
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
        workList.append(db->fetchValue(0).toString());
        begDate = minDate(begDate, db->fetchValue(1).toString());
        endDate = maxDate(endDate, db->fetchValue(2).toString());
    }

    int minLength = 999999;
    int mlNum = 0;

    for (int i=0; i<workList.size(); i++)
    {
        if (workList[i].size() < minLength) {
            minLength = workList[i].size();
            mlNum = i;
        }
    }
    firstWork = workList[mlNum];
    workList.removeAt(mlNum);
    for (int i=0; i<workList.size(); i++)
    {
        works += workList[i] + ", ";
    }
    works.chop(2);

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

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/avr-p1.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/avr-p1.html\n" + file.errorString());
        return result;
    }
    ts = new QTextStream(&file);
    page = ts->readAll();
    delete ts;
    file.close();

    page.replace("$USN$", unitShortName);
    page.replace("$BEGDATE$", begDate);
    page.replace("$ENDDATE$", endDate);
    page.replace("$DOCNUM$", docNum);
    page.replace("$SUBSYS$", subsystem);
    page.replace("$ORDERDATE$", orderDate);
    page.replace("$ORDERNUM$", orderNum);
    page.replace("$WT$", workType);
    page.replace("$FIRSTWORK$", firstWork);
    page.replace("$WORKS$", works);
    page.replace("$PLANBEGDATE$", planBegDate);
    page.replace("$PLANENDDATE$", planEndDate);
    page.replace("$ACCORDINGDOC$", "годовым графиком ремонта оборудования технологических систем " + schedNum);
    page.replace("$DOC1LABEL$", "-");
    page.replace("$DOC2LABEL$", "-");
    page.replace("$DOC3LABEL$", "");
    page.replace("$DOC1$", "Ведомость выполненых работ №33-20/" + docNum + " ВР");
    page.replace("$DOC2$", "Ведомость фактических затраченных материалов №33-20/" + docNum + " ВМ");
    page.replace("$DOC3$", "");

    result.append(page);

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/avr-p2.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/avr-p2.html\n" + file.errorString());
        return result;
    }
    ts = new QTextStream(&file);
    page = ts->readAll();
    delete ts;
    file.close();

    page.replace("$BEGDATE$", begDate);
    page.replace("$ENDDATE$", endDate);
    page.replace("$DOCNUM$", docNum);
    page.replace("$WORKS$", firstWork + ", " + works);
    page.replace("$OWNERLOC$", ownerLoc);
    page.replace("$OWNERNAME$", ownerName);
    page.replace("$MEMBER1LOC$", member1Loc);
    page.replace("$MEMBER2LOC$", member2Loc);
    page.replace("$MEMBER3LOC$", member3Loc);
    page.replace("$MEMBER1NAME", member1Name);
    page.replace("$MEMBER2NAME", member2Name);
    page.replace("$MEMBER3NAME", member3Name);
    page.replace("$REPAIRERLOC$", repairerLoc);
    page.replace("$REPAIRERNAME$", repairerName);
    page.replace("$CHIEFLOC$", chiefLoc);
    page.replace("$CHIEFNAME$", chiefName);

    result.append(page);

    return result;
}

QStringList KRReportPreviewForm::makeVVR(QString reportId)
{
    QStringList result;
    QString query = "SELECT "

    return result;
}
