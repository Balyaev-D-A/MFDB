#include "krreportpreviewform.h"
#include "ui_krreportpreviewform.h"
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

KRReportPreviewForm::KRReportPreviewForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::KRReportPreviewForm)
{
    ui->setupUi(this);
    connect(ui->saveButton, &QPushButton::clicked, this, &KRReportPreviewForm::saveButtonClicked);
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
    QString page, unitShortName, subsystem, date, begDate, endDate, planBegDate, planEndDate;
    QString orderDate, orderNum, schedNum, docNum, workType, firstWork, works, accordingDoc;
    QString ownerLoc, ownerName, member1Loc, member1Name, member2Loc, member2Name, member3Loc, member3Name;
    QString repairerLoc, repairerName, chiefLoc, chiefName, executor;
    QFile file;
    QTextStream *ts;
    QString query = "SELECT unit_shortname, unit_subsys, unit_schednum, krr_planbeg, krr_planend, krr_date, krr_docnum FROM krreports "
                    "LEFT JOIN units ON krr_unit = unit_id "
                    "WHERE krr_id = '%1'";
    query = query.arg(reportId);

    if (db->execQuery(query)) {
        if (db->nextRecord()) {
            unitShortName = db->fetchValue(0).toString();
            subsystem = db->fetchValue(1).toString();
            schedNum = db->fetchValue(2).toString();
            planBegDate = db->fetchValue(3).toString();
            planEndDate = db->fetchValue(4).toString();
            date = db->fetchValue(5).toString();
            docNum = db->fetchValue(6).toString();
        }
    } else {
        db->showError(this);
        return result;
    }

    query = "SELECT CONCAT_WS(' ', sch_name, sch_type, sch_kks), kr_begdate, kr_enddate FROM krrworks "
            "LEFT JOIN kaprepairs ON krw_work = kr_id "
            "LEFT JOIN schedule ON kr_sched = sch_id "
            "WHERE krw_report = '%1'";
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
    executor = db->getVariable("Исполнитель").toString();

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
    page.replace("$WT$", "КР");
    page.replace("$FIRSTWORK$", firstWork);
    page.replace("$WORKS$", works);
    page.replace("$PLANBEGDATE$", planBegDate);
    page.replace("$PLANENDDATE$", planEndDate);
    page.replace("$ACCORDINGDOC$", "годовым графиком ремонта оборудования технологических систем №" + schedNum);
    page.replace("$DOC1LABEL$", "-");
    page.replace("$DOC2LABEL$", "-");
    page.replace("$DOC3LABEL$", "");
    page.replace("$DOC1$", "Ведомость выполненых работ №33-20/" + docNum + " ВР");
    page.replace("$DOC2$", "Ведомость фактических затраченных материалов №33-20/" + docNum + " ВМ");
    page.replace("$DOC3$", "");
    page.replace("$EXECUTOR$", executor);

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

    page.replace("$USN$", unitShortName);
    page.replace("$BEGDATE$", begDate);
    page.replace("$ENDDATE$", endDate);
    page.replace("$DOCNUM$", docNum);
    page.replace("$WORKS$", firstWork + ", " + works);
    page.replace("$OWNERLOC$", ownerLoc);
    page.replace("$OWNERNAME$", ownerName);
    page.replace("$MEMBER1LOC$", member1Loc);
    page.replace("$MEMBER2LOC$", member2Loc);
    page.replace("$MEMBER3LOC$", member3Loc);
    page.replace("$MEMBER1NAME$", member1Name);
    page.replace("$MEMBER2NAME$", member2Name);
    page.replace("$MEMBER3NAME$", member3Name);
    page.replace("$REPAIRERLOC$", repairerLoc);
    page.replace("$REPAIRERNAME$", repairerName);
    page.replace("$CHIEFLOC$", chiefLoc);
    page.replace("$CHIEFNAME$", chiefName);
    page.replace("$DATE$", date);

    result.append(page);

    return result;
}

QStringList KRReportPreviewForm::makeVVR(QString reportId)
{
    typedef struct {
        QString name;
        QString type;
        QString kks;
        QString begDate;
        QString endDate;
    } Work;
    Work w;
    QList<Work> workList;
    QStringList result;
    QString unitShortName, subsys, date, docNum, works, currWork, begDate, endDate, workActions, workHours;
    QString ownerLoc, ownerName, repairerLoc, repairerName, chiefLoc, chiefName;
    QString pageTempNoSign, pageTempSign, page;
    QFile file;
    QTextStream *ts;
    int pageCount;
    QString query = "SELECT unit_shortname, unit_subsys, krr_date, krr_docnum FROM krreports "
                    "LEFT JOIN units ON krr_unit = unit_id "
                    "WHERE krr_id = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    if (db->nextRecord()) {
        unitShortName = db->fetchValue(0).toString();
        subsys = db->fetchValue(1).toString();
        date = db->fetchValue(2).toString();
        docNum = db->fetchValue(3).toString();
    }

    query = "SELECT sch_name, sch_type, sch_kks, kr_begdate, kr_enddate FROM krrworks "
            "LEFT JOIN kaprepairs ON krw_work = kr_id "
            "LEFT JOIN schedule ON kr_sched = sch_id "
            "WHERE krw_report = '%1'";
    query = query.arg(reportId);
    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    while (db->nextRecord())
    {
        works += db->fetchValue(0).toString() + " " + db->fetchValue(1).toString() + ", " + db->fetchValue(2).toString() + "; ";
        w.name = db->fetchValue(0).toString();
        w.type = db->fetchValue(1).toString();
        w.kks = db->fetchValue(2).toString();
        w.begDate = db->fetchValue(3).toString();
        w.endDate = db->fetchValue(4).toString();
        workList.append(w);
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

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/vvr-nosign.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/vvr-nosign.html\n" + file.errorString());
        return result;
    }
    ts = new QTextStream(&file);
    pageTempNoSign = ts->readAll();
    delete ts;
    file.close();

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/vvr-sign.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/vvr-sign.html\n" + file.errorString());
        return result;
    }
    ts = new QTextStream(&file);
    pageTempSign = ts->readAll();
    delete ts;
    file.close();

    pageCount = workList.size();

    for (int i=0; i<pageCount; i++)
    {
        if (i < pageCount-1) {
            page = pageTempNoSign;
        } else {
            page = pageTempSign;
        }

        currWork = "Капитальный ремонт: " + workList[i].name + " " + workList[i].type + ", " + workList[i].kks;

        query = "SELECT na_actions FROM normativactions WHERE na_dev = '%1' AND na_worktype = 'КР'";
        query = query.arg(workList[i].type);
        if (!db->execQuery(query)) {
            db->showError(this);
            return result;
        }
        if (db->nextRecord()) workActions = db->fetchValue(0).toString();
        else workActions = "";
        workActions.replace("\n", "<br/>");
        query = "SELECT nw_work FROM normativwork WHERE nw_dev = '%1' AND nw_worktype = 'КР'";
        query = query.arg(workList[i].type);
        if (!db->execQuery(query)) {
            db->showError(this);
            return result;
        }
        if (db->nextRecord()) workHours = db->fetchValue(0).toString();
        else workHours = "";

        page.replace("$USN$", unitShortName);
        page.replace("$DOCNUM$", docNum);
        page.replace("$SUBSYS$", subsys);
        page.replace("$PAGE$", QString("%1").arg(i+1));
        page.replace("$PAGECOUNT$", QString("%1").arg(pageCount));
        page.replace("$WORKS$", works);
        page.replace("$BEGDATE$", workList[i].begDate);
        page.replace("$ENDDATE$", workList[i].endDate);
        page.replace("$WT$", "КР");
        page.replace("$CURRWORK$", currWork);
        page.replace("$ACTIONS$", workActions);
        page.replace("$HOURS$", workHours.replace(".", ","));

        if (i == pageCount-1) {
            page.replace("$OWNERLOC$", ownerLoc);
            page.replace("$OWNERNAME$", ownerName);
            page.replace("$REPAIRERLOC$", repairerLoc);
            page.replace("$REPAIRERNAME$", repairerName);
            page.replace("$CHIEFLOC$", chiefLoc);
            page.replace("$CHIEFNAME$", chiefName);
            page.replace("$DATE$", date);
        }
        result.append(page);
    }

    return result;
}

QStringList KRReportPreviewForm::makeVFZM(QString reportId)
{
    const int fillerNS = 684;
    const int fillerS = 596;
    const int head1Height = 44;
    const int head2Height = 56;
    const int rowDefaultHeight = 23;
    const int cellWidth = 317;
    typedef struct {
        QString krId;
        QString name;
        QString type;
        QString kks;
        QString begDate;
        QString endDate;
    } Work;
    Work w;
    QList<Work> workList;
    QList<QStringList> matTable;
    QStringList matRow;
    QStringList pages;
    int pageCount, currBlockSize, allBlocksSize, foundRow, strCount;
    QFile file;
    QTextStream *ts;
    QStringList result;
    QString page, pageTempNoSign, pageTempSign, rows, workRows, unitShortName, subsystem, date, docNum, begDate, endDate, works, oesn, matName, matDoc;
    QString repairerLoc, repairerName, chiefLoc, chiefName;
    QString header1Temp = "<tr class=\"vfzmheader1\">"
                          "<td colspan=\"13\" class=\"hc it\">$WORK$</td>"
                          "</tr>\n";
    QString header2Temp = "<tr class=\"vfzmheader2\">"
                          "<td></td>"
                          "<td colspan=\"7\"></td>"
                          "<td></td>"
                          "<td colspan=\"2\" class=\"hc vc f10\">$OESN$</td>"
                          "<td colspan=\"2\"></td>"
                          "</tr>\n";
    QString rowTemp = "<tr class=\"vfzmrow\">"
                      "<td class=\"hr\">$MNUM$</td>"
                      "<td colspan=\"7\">$MATERIAL$</td>"
                      "<td class=\"hc\">$MEAS$</td>"
                      "<td colspan=\"2\" class=\"hc\">$NORMCOUNT$</td>"
                      "<td colspan=\"2\" class=\"hc\">$REALCOUNT$</td>"
                      "</tr>\n";
    QFontMetrics *fm = new QFontMetrics(QFont("Times New Roman", 12));
    QRect bounding;
    QString query = "SELECT unit_shortname, unit_subsys, krr_date, krr_docnum FROM krreports "
                    "LEFT JOIN units ON krr_unit = unit_id "
                    "WHERE krr_id = '%1'";
    query = query.arg(reportId);

    if (db->execQuery(query)) {
        if (db->nextRecord()) {
            unitShortName = db->fetchValue(0).toString();
            subsystem = db->fetchValue(1).toString();
            date = db->fetchValue(2).toString();
            docNum = db->fetchValue(3).toString();
        }
    } else {
        db->showError(this);
        return result;
    }

    query = "SELECT sch_name, sch_type, sch_kks, kr_begdate, kr_enddate, krw_work FROM krrworks "
            "LEFT JOIN kaprepairs ON krw_work = kr_id "
            "LEFT JOIN schedule ON kr_sched = sch_id "
            "WHERE krw_report = '%1'";
    query = query.arg(reportId);
    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    while (db->nextRecord())
    {
        works += db->fetchValue(0).toString() + " " + db->fetchValue(1).toString() + ", " + db->fetchValue(2).toString() + "; ";
        w.name = db->fetchValue(0).toString();
        w.type = db->fetchValue(1).toString();
        w.kks = db->fetchValue(2).toString();
        w.begDate = db->fetchValue(3).toString();
        w.endDate = db->fetchValue(4).toString();
        w.krId = db->fetchValue(5).toString();
        workList.append(w);
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

    currBlockSize = 0;
    allBlocksSize = 0;
    begDate = "99.99.9999";
    endDate = "00.00.0000";
    for (int i=0; i<workList.size(); i++)
    {
        begDate = minDate(begDate, workList[i].begDate);
        endDate = maxDate(endDate, workList[i].endDate);

        workRows = "";
        workRows += header1Temp;
        currBlockSize += head1Height;
        workRows.replace("$WORK$", workList[i].name + " " + workList[i].type + ", " + workList[i].kks);
        QString query = "SELECT nw_oesn FROM normativwork WHERE nw_dev = '%1' AND nw_worktype = 'КР'";
        query = query.arg(workList[i].type);
        if (!db->execQuery(query)) {
            db->showError(this);
            return result;
        }
        if (db->nextRecord()) {
            oesn = db->fetchValue(0).toString();
        } else {
            oesn = "";
        }
        workRows += header2Temp;
        workRows.replace("$OESN$", oesn);
        currBlockSize += head2Height;
        query = "SELECT nm_material, mat_name, mat_doc, mat_measure, nm_count FROM normativmat "
                "LEFT JOIN materials ON nm_material = mat_id "
                "WHERE nm_dev = '%1' AND nm_worktype = 'КР'";
        query = query.arg(workList[i].type);
        if (!db->execQuery(query)) {
            db->showError(this);
            return result;
        }
        matTable.clear();
        while (db->nextRecord())
        {
            matRow.clear();
            matRow.append(db->fetchValue(0).toString());
            matName = db->fetchValue(1).toString();
            matDoc = db->fetchValue(2).toString();
            matName = matName.simplified();
            if (!matName.endsWith(".")) matName += ".";
            matRow.append(matName + " " + matDoc);
            matRow.append(db->fetchValue(3).toString());
            matRow.append(db->fetchValue(4).toString());
            matRow.append(db->fetchValue(4).toString());
            matTable.append(matRow);
        }

        query = "SELECT kam_material, mat_name, mat_doc, mat_measure, kam_count FROM kradditionalmats "
                "LEFT JOIN materials ON kam_material = mat_id "
                "WHERE kam_kr = '%1'";
        query = query.arg(workList[i].krId);

        if (!db->execQuery(query)) {
            db->showError(this);
            return result;
        }

        while (db->nextRecord())
        {
            foundRow = -1;
            for (int j=0; j<matTable.size(); j++)
            {
                if (matTable[j][0] == db->fetchValue(0).toString()) foundRow = i;
            }

            if (foundRow < 0) {
                matRow.clear();
                matRow.append(db->fetchValue(0).toString());
                matName = db->fetchValue(1).toString();
                matDoc = db->fetchValue(2).toString();
                matName = matName.simplified();
                if (!matName.endsWith(".")) matName += ".";
                matRow.append(matName + " " + matDoc);
                matRow.append(db->fetchValue(3).toString());
                matRow.append("-");
                matRow.append(db->fetchValue(4).toString());
                matTable.append(matRow);
            } else {
                if (db->fetchValue(4).toFloat() == 0.00) {
                    matTable.removeAt(foundRow);
                } else {
                    matTable[foundRow][4] = db->fetchValue(4).toString();
                }
            }
        }

        for (int j=0; j<matTable.size(); j++)
        {
            workRows += rowTemp;
            workRows.replace("$MNUM$", QString("%1").arg(j+1));
            workRows.replace("$MATERIAL$", matTable[j][1]);
            workRows.replace("$MEAS$", matTable[j][2]);
            workRows.replace("$NORMCOUNT$", matTable[j][3].replace(".", ","));
            workRows.replace("$REALCOUNT$", matTable[j][4].replace(".", ","));
            bounding = fm->boundingRect(0, 0, cellWidth, fillerNS, Qt::TextWordWrap | Qt::AlignLeft, matTable[j][1]);
            strCount = (bounding.height() + 1)/20;
            if (strCount == 1)
                currBlockSize += rowDefaultHeight;
            else
                currBlockSize += (strCount * 19);
            qDebug() << matTable[j][1] << " " << strCount;
        }

        if (i == workList.size()-1) {
            if (allBlocksSize + currBlockSize <= fillerS) {
                allBlocksSize += currBlockSize;
                rows += workRows;
                workRows = "";
                pages.append(rows);
            } else {
                pages.append(rows);
                rows = "";
                rows += workRows;
                workRows = "";
                allBlocksSize = 0;
                pages.append(rows);
                rows = "";
            }
        } else {
            if (allBlocksSize + currBlockSize <= fillerNS) {
                allBlocksSize += currBlockSize;
                currBlockSize = 0;
                rows += workRows;
                workRows = "";
            } else {
                pages.append(rows);
                rows = "";
                rows += workRows;
                workRows = "";
                allBlocksSize = 0;
                currBlockSize = 0;
            }
        }
    }

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/vfzm-nosign.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/vfzm-nosign.html\n" + file.errorString());
        return result;
    }
    ts = new QTextStream(&file);
    pageTempNoSign = ts->readAll();
    delete ts;
    file.close();

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/vfzm-sign.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/vfzm-sign.html\n" + file.errorString());
        return result;
    }
    ts = new QTextStream(&file);
    pageTempSign = ts->readAll();
    delete ts;
    file.close();

    pageCount = pages.size();

    for (int i=0; i<pages.size(); i++)
    {
        if (i < pages.size()-1)
            page = pageTempNoSign;
        else
            page = pageTempSign;

        page.replace("$BEGDATE$", begDate);
        page.replace("$ENDDATE$", endDate);
        page.replace("$USN$", unitShortName);
        page.replace("$PAGECOUNT$", QString("%1").arg(pageCount));
        page.replace("$PAGE$", QString("%1").arg(i+1));
        page.replace("$SUBSYS$", subsystem);
        page.replace("$DOCNUM$", docNum);
        page.replace("$WORKS$", works);
        page.replace("$ROWS$", pages[i]);

        if (i == pages.size()-1) {
            page.replace("$DATE$", date);
            page.replace("$REPAIRERLOC$", repairerLoc);
            page.replace("$REPAIRERNAME$", repairerName);
            page.replace("$CHIEFLOC$", chiefLoc);
            page.replace("$CHIEFNAME$", chiefName);
        }
        result.append(page);
    }

    return result;
}

void KRReportPreviewForm::showPreview(QString reportId)
{
    QString body, page;
    QStringList avr, vvr, vfzm;
    QFile file;
    QTextStream *ts;

    avr = makeAVR(reportId);
    vvr = makeVVR(reportId);
    vfzm = makeVFZM(reportId);

    for (int i=0; i<avr.size(); i++)
        body += avr[i];
    for (int i=0; i<vvr.size(); i++)
        body += vvr[i];
    for (int i=0; i<vfzm.size(); i++)
        body += vfzm[i];

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/report.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/report.html\n" + file.errorString());
        return;
    }
    ts = new QTextStream(&file);
    page = ts->readAll();
    delete ts;
    file.close();

    page.replace("$BODY$", body);

    currentPage = page;
    ui->htmlView->setHtml(page);
    show();
}

void KRReportPreviewForm::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);

    emit closed(this);
}

void KRReportPreviewForm::saveButtonClicked()
{
    QFile file;
    QString fileName;

    fileName = QFileDialog::getSaveFileName(this, "Выберите файл для сохранения", QApplication::applicationDirPath(), "HTML файлы (*.htm *.html)");
    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть файл: \n" + file.errorString());
        return;
    }
    file.write(currentPage.toUtf8());
    file.close();
}
