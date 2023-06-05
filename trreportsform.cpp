#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include<QJsonArray>
#include "trreportsform.h"
#include "ui_trreportsform.h"

TRReportsForm::TRReportsForm(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::TRReportsForm)
{
    ui->setupUi(this);
    ui->table->hideColumn(0);
    connect(ui->addButton, &QToolButton::clicked, this, &TRReportsForm::addButtonClicked);
    connect(ui->updateButton, &QToolButton::clicked, this, &TRReportsForm::updateReports);
    connect(ui->editButton, &QToolButton::clicked, this, &TRReportsForm::editButtonClicked);
    connect(ui->deleteButton, &QToolButton::clicked, this, &TRReportsForm::deleteButtonClicked);
    connect(ui->saveButton, &QToolButton::clicked, this, &TRReportsForm::saveButtonClicked);
    connect(ui->table, &QTableWidget::cellDoubleClicked, this, &TRReportsForm::tableCellDoubleClicked);
}

TRReportsForm::~TRReportsForm()
{
    delete ui;
}

void TRReportsForm::setDatabase(Database *db)
{
    this->db = db;
}

void TRReportsForm::updateReports()
{
    int curRow;
    QString query = "SELECT trr_id, trr_date, unit_name, trr_desc FROM trreports "
                    "LEFT JOIN units ON trr_unit = unit_id";

    while (ui->table->rowCount()) ui->table->removeRow(0);

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }

    ui->table->setSortingEnabled(false);
    while (db->nextRecord())
    {
        curRow = ui->table->rowCount();
        ui->table->insertRow(curRow);
        for (int i=0; i<4; i++)
        {
            if (i == 1) {
                QTableWidgetItem *it = new QTableWidgetItem();
                it->setData(Qt::EditRole, QDate::fromString(db->fetchValue(i).toString(), "dd.MM.yyyy"));
                ui->table->setItem(curRow, i, it);
                continue;
            }
            ui->table->setItem(curRow, i, new QTableWidgetItem(db->fetchValue(i).toString()));
        }
    }
    ui->table->setSortingEnabled(true);
    ui->table->sortByColumn(1, Qt::AscendingOrder);
    ui->table->resizeColumnsToContents();
    ui->table->scrollToBottom();
}

void TRReportsForm::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateReports();
}

void TRReportsForm::addButtonClicked()
{
    TRReportForm *trf = new TRReportForm(this);
    trf->setDatabase(db);
    connect(trf, &TRReportForm::closed, this, &TRReportsForm::reportFormClosed);
    connect(trf, &TRReportForm::saved, this, &TRReportsForm::updateReports);
    trf->show();
}

void TRReportsForm::editButtonClicked()
{
    if (!ui->table->currentItem()) return;
    TRReportForm *trf = new TRReportForm(this);
    trf->setDatabase(db);
    trf->editReport(ui->table->item(ui->table->currentRow(), 0)->text());
    connect(trf, &TRReportForm::closed, this, &TRReportsForm::reportFormClosed);
    connect(trf, &TRReportForm::saved, this, &TRReportsForm::updateReports);
    trf->show();
}

void TRReportsForm::reportFormClosed(TRReportForm *sender)
{
    disconnect(sender, &TRReportForm::closed, this, &TRReportsForm::reportFormClosed);
    disconnect(sender, &TRReportForm::saved, this, &TRReportsForm::updateReports);
    sender->deleteLater();
}

void TRReportsForm::deleteButtonClicked()
{
    if (!ui->table->currentItem()) return;

    QString query = "DELETE FROM trreports WHERE trr_id = '%1'";
    query = query.arg(ui->table->item(ui->table->currentRow(), 0)->text());

    if (!db->execQuery(query)) {
        db->showError(this);
        return;
    }
    ui->table->removeRow(ui->table->currentRow());
}



void TRReportsForm::saveButtonClicked()
{
    QString body, page;
    QStringList avr, vvr, vfzm, ado, po;
    QFile file;
    QTextStream *ts;
    QString reportId, fileName;

    if (!ui->table->currentItem()) return;

    reportId = ui->table->item(ui->table->currentRow(), 0)->text();

    avr = makeAVR(reportId);
    vvr = makeVVR(reportId);
    vfzm = makeVFZM(reportId);
    ado = makeADO(reportId);

    for (int i=0; i<avr.size(); i++)
        body += avr[i];
    for (int i=0; i<vvr.size(); i++)
        body += vvr[i];
    for (int i=0; i<vfzm.size(); i++)
        body += vfzm[i];
    for (int i=0; i<ado.size(); i++)
        body += ado[i];
    if (vvr.size() > 4) {
        po = makePO(reportId);
        for (int i=0; i<po.size(); i++)
            body += po[i];
    }

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

    fileName = QFileDialog::getSaveFileName(this, "Выберите файл для сохранения", lastSavedDir + "ТР " + ui->table->item(ui->table->currentRow(), 2)->text() + ".htm", "HTML файлы (*.htm *.html)");
    file.setFileName(fileName);
    lastSavedDir = QFileInfo(fileName).absolutePath() + "/";
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть файл: \n" + file.errorString());
        return;
    }
    file.write(page.toUtf8());
    file.close();
}

QString TRReportsForm::minDate(QString date1, QString date2)
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

QString TRReportsForm::maxDate(QString date1, QString date2)
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

int TRReportsForm::maxInt(int first, int second)
{
    if (first > second) return first;
    else return second;
}

QStringList TRReportsForm::makeAVR(QString reportId)
{
    QStringList result;
    QStringList workList, ktdList, opcardNums, devTypeList;
    QString page, unitShortName, subsystem, date, begDate, endDate, planBegDate, planEndDate;
    QString orderDate, orderNum, schedNum, docNum, workType, firstWork, otherWorks, works, accordingDoc;
    QString ownerLoc, ownerName, member1Loc, member1Name, member2Loc, member2Name, member3Loc, member3Name;
    QString repairerLoc, repairerName, chiefLoc, chiefName, executor, defNums, ktdText, prepQuery, reglament;
    QFile file;
    QTextStream *ts;
    QFontMetrics *fm = new QFontMetrics(QFont("Times New Roman", 12));
    QRect bounding;
    QStringList fwWordList;
    int worksCount;
    int breakPos;
    bool isFit = false;
    const int firstWorkWidth = 414;
    QString member4Loc = "", member4Name = "", member5Loc = "", member5Name = "";
    QString additionalMemberRow = "<tr class=\"avr2row18\">\n"
            "<td class=\"border-l\"></td>\n"
            "<td colspan=\"3\" class=\"border-none\">%1</td>\n"
            "<td colspan=\"2\" class=\"hc border-tb f6 vt\">(подпись)</td>\n"
            "<td class=\"border-none hc\">%2</td>\n"
            "<td colspan=\"2\" class=\"border-r\">%3</td>\n"
            "</tr>\n";
    QString prepAddMemberRow, addMembers = "";
    QString query = "SELECT unit_shortname, unit_subsys, unit_schednum, trr_planbeg, trr_planend, trr_date, trr_docnum FROM trreports "
                    "LEFT JOIN units ON trr_unit = unit_id "
                    "WHERE trr_id = '%1'";
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

    query = "SELECT def_devname, def_devtype, def_kks, def_begdate, def_enddate, def_num, ktd_doc FROM trrworks "
            "LEFT JOIN defects ON trw_work = def_id "
            "LEFT JOIN ktd ON def_devtype = ktd_dev "
            "WHERE trw_report = '%1' ORDER BY trw_order";
    query = query.arg(reportId);
    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    begDate = "99.99.9999";
    endDate = "00.00.0000";
    while (db->nextRecord())
    {
        workList.append(db->fetchValue(0).toString() + " " + db->fetchValue(1).toString() + ", " + db->fetchValue(2).toString());
        begDate = minDate(begDate, db->fetchValue(3).toString());
        endDate = maxDate(endDate, db->fetchValue(4).toString());
        defNums += db->fetchValue(5).toString() + ", ";
        if (!devTypeList.contains(db->fetchValue(1).toString()))
            devTypeList.append(db->fetchValue(1).toString());
    }
    defNums.chop(2);

    reglament = db->getVariable("РегламентСокр").toString();

    prepQuery = "SELECT nw_ktdshort FROM normativwork WHERE nw_dev = '%1' AND nw_worktype = 'ТР'";

    for (int i=0; i<devTypeList.count(); i++)
    {
        query = prepQuery.arg(devTypeList[i]);
        if (!db->execQuery(query)) {
            db->showError(this);
            return result;
        }
        if (db->nextRecord()) {
            if (db->fetchValue(0).toString().simplified().isEmpty()) {
                if (!ktdList.contains(reglament))
                    ktdList.append(reglament);
            }
            else {
                if (!ktdList.contains(db->fetchValue(0).toString()))
                    ktdList.append(db->fetchValue(0).toString());
            }
        }
    }

    for (int i=0; i<ktdList.size(); i++)
    {
        if (ktdList[i].contains("Операционная карта №"))
            opcardNums.append(ktdList[i].split("№")[1]);
    }
    if (opcardNums.size() > 1) {
        ktdText.append("Операционные карты<br/>№№");
        for (int i=0; i<opcardNums.size(); i++)
        {
           ktdText.append(opcardNums[i] + ", ");
        }
        ktdText.chop(2);
        ktdText.append(";<br/>");
        for (int i=0; i<ktdList.size(); i++)
        {
            if (ktdList[i].contains("Операционная"))
                continue;
            ktdText.append(ktdList[i]+";<br/>");
        }
        ktdText.chop(5); // убираем последний <br/>
    }
    else {
        for (int i=0; i<ktdList.size(); i++)
        {
            ktdText.append(ktdList[i]+";<br/>");
        }
    ktdText.chop(5); // убираем последний <br/>
    }

    worksCount = workList.size();
    if (worksCount > 4) {
        firstWork = "Согласно перечню оборудования к акту №%1 (Количество %2 шт.).";
        firstWork = firstWork.arg(docNum).arg(worksCount);
    }
    else {
        firstWork = workList[0];
        firstWork += ";";
    }

    bounding = fm->boundingRect(firstWork); 
    if (bounding.width() > firstWorkWidth) {
        fwWordList = firstWork.split(" ");
        breakPos = fwWordList.size();
        otherWorks = "";
        while (!isFit && breakPos)
        {
            breakPos--;
            firstWork = "";
            for (int i=0; i<breakPos; i++)
              firstWork  += fwWordList[i] + " ";
            firstWork.chop(1);
            bounding = fm->boundingRect(firstWork);
            isFit = bounding.width() < firstWorkWidth;
        }
        for (int i=breakPos; i<fwWordList.size(); i++)
            otherWorks += fwWordList[i] + " ";
    }

    if (worksCount > 4) {
        works = "Согласно перечню оборудования к акту №%1 (Количество %2 шт.).";
        works = works.arg(docNum).arg(worksCount);
    }
    else {
        for (int i=1; i<workList.size(); i++)
            otherWorks += workList[i] + "; ";
        otherWorks.chop(2);

        for (int i=0; i<workList.size(); i++)
        {
            works += workList[i] + "; ";
        }
        works.chop(2);
    }

    query = "SELECT sig_loc, sig_name, trs_role FROM trsigners "
            "LEFT JOIN signers ON trs_signer = sig_id "
            "WHERE trs_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    while (db->nextRecord())
    {
        switch (db->fetchValue(2).toInt()) {
        case TRSOWNER:
            ownerLoc = db->fetchValue(0).toString();
            ownerName = db->fetchValue(1).toString();
            break;
        case TRSMEMBER1:
            member1Loc = db->fetchValue(0).toString();
            member1Name = db->fetchValue(1).toString();
            break;
        case TRSMEMBER2:
            member2Loc = db->fetchValue(0).toString();
            member2Name = db->fetchValue(1).toString();
            break;
        case TRSMEMBER3:
            member3Loc = db->fetchValue(0).toString();
            member3Name = db->fetchValue(1).toString();
            break;
        case TRSMEMBER4:
            member4Loc = db->fetchValue(0).toString();
            member4Name = db->fetchValue(1).toString();
            break;
        case TRSMEMBER5:
            member5Loc = db->fetchValue(0).toString();
            member5Name = db->fetchValue(1).toString();
            break;
        case TRSREPAIRER:
            repairerLoc = db->fetchValue(0).toString();
            repairerName = db->fetchValue(1).toString();
            break;
        case TRSCHIEF:
            chiefLoc = db->fetchValue(0).toString();
            chiefName = db->fetchValue(1).toString();
            break;
        }
    }

    if (member2Name != "") {
        prepAddMemberRow = additionalMemberRow.arg(member2Loc).arg(date).arg(member2Name);
        addMembers += prepAddMemberRow;
    }

    if (member3Name != "") {
        prepAddMemberRow = additionalMemberRow.arg(member3Loc).arg(date).arg(member3Name);
        addMembers += prepAddMemberRow;
    }

    if (member4Name != "") {
        prepAddMemberRow = additionalMemberRow.arg(member4Loc).arg(date).arg(member4Name);
        addMembers += prepAddMemberRow;
    }

    if (member5Name != "") {
        prepAddMemberRow = additionalMemberRow.arg(member5Loc).arg(date).arg(member5Name);
        addMembers += prepAddMemberRow;
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
    page.replace("$WT$", "ТР");
    page.replace("$FIRSTWORK$", firstWork);
    page.replace("$OTHERWORKS$", otherWorks);
    page.replace("$WORKS$", works);
    page.replace("$PLANBEGDATE$", planBegDate);
    page.replace("$PLANENDDATE$", planEndDate);
    page.replace("$ACCORDINGDOC$", "записями в общестанционном журнале дефектов \"SAP Logon\" №№ " + defNums);
    page.replace("$DOC1LABEL$", "-");
    page.replace("$DOC2LABEL$", "-");
    page.replace("$DOC3LABEL$", "-");
    page.replace("$DOC4LABEL$", "");
    page.replace("$DOC1$", "Ведомость выполненых работ №" + docNum + " ВР;");
    page.replace("$DOC2$", "Ведомость фактических затраченных материалов №" + docNum + " ВМ;");
    page.replace("$DOC3$", "Акт о дефектах оборудования №" + docNum + " АД;");
    page.replace("$DOC4$", "");
    page.replace("$EXECUTOR$", executor);
    page.replace("$KTDTEXT$", ktdText);

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
    page.replace("$WORKS$", works);
    page.replace("$OWNERLOC$", ownerLoc);
    page.replace("$OWNERNAME$", ownerName);
    page.replace("$MEMBER1LOC$", member1Loc);
    page.replace("$MEMBER1NAME$", member1Name);
    page.replace("$AMEMBERS$", addMembers);
    page.replace("$REPAIRERLOC$", repairerLoc);
    page.replace("$REPAIRERNAME$", repairerName);
    page.replace("$CHIEFLOC$", chiefLoc);
    page.replace("$CHIEFNAME$", chiefName);
    page.replace("$DATE$", date);

    result.append(page);

    return result;
}

QStringList TRReportsForm::makeVVR(QString reportId)
{
    typedef struct {
        QString name;
        QString type;
        QString kks;
        QString begDate;
        QString endDate;
        QString journalDesc;
        QString realDesc;
        QString repairDesc;
        QString actionsDesc;
        QString ktdDoc;
    } Work;
    Work w;
    QList<Work> workList;
    QStringList result;
    QString unitShortName, subsys, date, docNum, works, currWork, begDate, endDate, workActions, workHours;
    QString ownerLoc, ownerName, repairerLoc, repairerName, chiefLoc, chiefName;
    QString pageTempNoSign, pageTempSign, page, ktdText, reglament;
    QFile file;
    QTextStream *ts;
    int pageCount;
    QStringList workActList;
    QString query = "SELECT unit_shortname, unit_subsys, trr_date, trr_docnum FROM trreports "
                    "LEFT JOIN units ON trr_unit = unit_id "
                    "WHERE trr_id = '%1'";
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

    reglament = db->getVariable("РегламентПолн").toString();

    query = "SELECT def_devname, def_devtype, def_kks, def_begdate, def_enddate, def_journaldesc, def_realdesc, def_repairdesc, def_actionsdesc FROM trrworks "
            "LEFT JOIN defects ON trw_work = def_id "
            "LEFT JOIN ktd ON def_devtype = ktd_dev "
            "WHERE trw_report = '%1' ORDER BY trw_order";
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
        w.journalDesc = db->fetchValue(5).toString();
        w.realDesc = db->fetchValue(6).toString();
        w.repairDesc = db->fetchValue(7).toString();
        w.actionsDesc = db->fetchValue(8).toString();
        workList.append(w);
    }
    works.chop(2);

    if (workList.size() > 4) {
        works = "Согласно перечню оборудования к акту №%1 (Количество %2 шт.).";
        works = works.arg(docNum).arg(workList.size());
    }

    query = "SELECT sig_loc, sig_name, trs_role FROM trsigners "
            "LEFT JOIN signers ON trs_signer = sig_id "
            "WHERE trs_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    while (db->nextRecord())
    {
        switch (db->fetchValue(2).toInt()) {
        case TRSOWNER:
            ownerLoc = db->fetchValue(0).toString();
            ownerName = db->fetchValue(1).toString();
            break;
        case TRSREPAIRER:
            repairerLoc = db->fetchValue(0).toString();
            repairerName = db->fetchValue(1).toString();
            break;
        case TRSCHIEF:
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

        currWork = "Текущий ремонт: " + workList[i].name + " " + workList[i].type + ", " + workList[i].kks + "\n";

        query = "SELECT nw_work, nw_ktd FROM normativwork WHERE nw_dev = '%1' AND nw_worktype = 'ТР'";
        query = query.arg(workList[i].type);
        if (!db->execQuery(query)) {
            db->showError(this);
            return result;
        }
        if (db->nextRecord()) {
            workHours = db->fetchValue(0).toString();
            ktdText = db->fetchValue(1).toString();
        }
        else {
            workHours = "";
            ktdText = reglament;
        }

        if (ktdText.simplified() == "")
            ktdText = reglament;

        page.replace("$USN$", unitShortName);
        page.replace("$DOCNUM$", docNum);
        page.replace("$SUBSYS$", subsys);
        page.replace("$PAGE$", QString("%1").arg(i+1));
        page.replace("$PAGECOUNT$", QString("%1").arg(pageCount));
        page.replace("$WORKS$", works);
        page.replace("$KTDTEXT$", ktdText);
        page.replace("$BEGDATE$", workList[i].begDate);
        page.replace("$ENDDATE$", workList[i].endDate);
        page.replace("$WT$", "ТР");
        page.replace("$CURRWORK$", currWork);
        page.replace("$ACTIONS$", workList[i].actionsDesc.replace("\n", "<br/>"));
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

QStringList TRReportsForm::makeVFZM(QString reportId)
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
    int pageCount, currBlockSize, allBlocksSize, strCount;
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
    QString query = "SELECT unit_shortname, unit_subsys, trr_date, trr_docnum FROM trreports "
                    "LEFT JOIN units ON trr_unit = unit_id "
                    "WHERE trr_id = '%1'";
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

    query = "SELECT def_devname, def_devtype, def_kks, def_begdate, def_enddate, trw_work FROM trrworks "
            "LEFT JOIN defects ON trw_work = def_id "
            "WHERE trw_report = '%1' ORDER BY trw_order";
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

    if (workList.size() > 4) {
        works = "Согласно перечню оборудования к акту №%1 (Количество %2 шт.).";
        works = works.arg(docNum).arg(workList.size());
    }

    query = "SELECT sig_loc, sig_name, trs_role FROM trsigners "
            "LEFT JOIN signers ON trs_signer = sig_id "
            "WHERE trs_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    while (db->nextRecord())
    {
        switch (db->fetchValue(2).toInt()) {
        case TRSREPAIRER:
            repairerLoc = db->fetchValue(0).toString();
            repairerName = db->fetchValue(1).toString();
            break;
        case TRSCHIEF:
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

        QString query = "SELECT nw_oesn FROM normativwork WHERE nw_dev = '%1' AND nw_worktype = 'ТР'";
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

        matTable.clear();
        query = "SELECT dam_material, mat_name, mat_doc, mat_measure, dam_oesn, dam_count FROM defadditionalmats "
                "LEFT JOIN materials ON dam_material = mat_id "
                "WHERE dam_defect = '%1' ORDER BY dam_order";
        query = query.arg(workList[i].krId);

        if (!db->execQuery(query)) {
            db->showError(this);
            return result;
        }

        if (db->affectedRows() > 0) {
            workRows += header1Temp;
            currBlockSize += head1Height;
            workRows.replace("$WORK$", workList[i].name + " " + workList[i].type + ", " + workList[i].kks);
            workRows += header2Temp;
            workRows.replace("$OESN$", oesn);
            currBlockSize += head2Height;
        }

        while (db->nextRecord())
        {
            if (db->fetchValue(5).toFloat() == 0) continue;
            matRow.clear();
            matRow.append(db->fetchValue(0).toString());
            matName = db->fetchValue(1).toString();
            matDoc = db->fetchValue(2).toString();
            matName = matName.simplified();
            if (!matName.endsWith(".")) matName += ".";
            matRow.append(matName + " " + matDoc);
            matRow.append(db->fetchValue(3).toString());
            if (db->fetchValue(4).toFloat() == 0) matRow.append("-");
            else matRow.append(db->fetchValue(4).toString());
            matRow.append(db->fetchValue(5).toString());
            matTable.append(matRow);
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
                allBlocksSize = currBlockSize;
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

QStringList TRReportsForm::makeADO(QString reportId)
{
    typedef struct {
        QString name;
        QString type;
        QString kks;
        QString techDoc;
        QString defect;
        QString repair;
    }  Work;
    const int fillerNS = 710;
    const int memberRowHeight = 27;
    int fillerS = 420;
    const QRect workRect(0, 0, 132, 107);
    const QRect tdocRect(0, 0, 173,107);
    const QRect defectRect(0, 0, 186, 107);
    const QRect repairRect(0, 0, 129, 107);
    int rowHeight, allRowsHeight, strCount, pageCount;
    QFontMetrics *fm = new QFontMetrics(QFont("Times New Roman", 12));
    QRect bounding;
    QStringList result;
    Work w;
    QList<Work> workList;
    QStringList tblRow;
    QString unitShortName, subsystem, docNum, date, page, tdocStr, work, works, row, rows, pageTempSign, pageTempNoSign;
    QString ownerLoc, ownerName, member1Loc, member1Name, member2Loc, member2Name, member3Loc, member3Name, repairerLoc, repairerName, chiefLoc, chiefName;
    QStringList pages;
    QFile file;
    QTextStream *ts;
    QString prepQuery;
    QString member4Loc = "", member4Name = "", member5Loc = "", member5Name = "";
    QString addMembers = "";
    QString rowTmpl = "<tr class=\"adorow\">\n"
                      "<td colspan=\"3\" class=\"hc\">$WORK$</td>\n"
                      "<td colspan=\"2\" class=\"hc\">Обеспечение работоспособности в соответствии с $TECHDOC$</td>\n"
                      "<td colspan=\"2\" class=\"hc\">$DEFECT$</td>\n"
                      "<td colspan=\"3\" class=\"hc\">$REPAIR$</td>\n"
                      "</tr>\n";
    QString additionalMemberRow ="<tr class=\"adorow12\">\n"
            "<td class=\"border-l\"></td>\n"
            "<td colspan=\"3\" class=\"border-none\">%1</td>\n"
            "<td colspan=\"2\" class=\"hc f6 vt border-tb\">(подпись)</td>\n"
            "<td class=\"border-none\">%2</td>\n"
            "<td colspan=\"3\" class=\"border-r\">%3</td>\n"
            "</tr>\n";

    QString query = "SELECT unit_shortname, unit_subsys, trr_date, trr_docnum FROM trreports "
                    "LEFT JOIN units ON trr_unit = unit_id "
                    "WHERE trr_id = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    if (db->nextRecord()) {
        unitShortName = db->fetchValue(0).toString();
        subsystem = db->fetchValue(1).toString();
        date = db->fetchValue(2).toString();
        docNum = db->fetchValue(3).toString();
    }

    query = "SELECT sig_loc, sig_name, trs_role FROM trsigners "
            "LEFT JOIN signers ON trs_signer = sig_id "
            "WHERE trs_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    while (db->nextRecord())
    {
        switch (db->fetchValue(2).toInt()) {
        case TRSOWNER:
            ownerLoc = db->fetchValue(0).toString();
            ownerName = db->fetchValue(1).toString();
            break;
        case TRSMEMBER1:
            member1Loc = db->fetchValue(0).toString();
            member1Name = db->fetchValue(1).toString();
            break;
        case TRSMEMBER2:
            member2Loc = db->fetchValue(0).toString();
            member2Name = db->fetchValue(1).toString();
            break;
        case TRSMEMBER3:
            member3Loc = db->fetchValue(0).toString();
            member3Name = db->fetchValue(1).toString();
            break;
        case TRSMEMBER4:
            member4Loc = db->fetchValue(0).toString();
            member4Name = db->fetchValue(1).toString();
            break;
        case TRSMEMBER5:
            member5Loc = db->fetchValue(0).toString();
            member5Name = db->fetchValue(1).toString();
            break;
        case TRSREPAIRER:
            repairerLoc = db->fetchValue(0).toString();
            repairerName = db->fetchValue(1).toString();
            break;
        case TRSCHIEF:
            chiefLoc = db->fetchValue(0).toString();
            chiefName = db->fetchValue(1).toString();
            break;
        }
    }

    if (member2Name != "") {
        addMembers += additionalMemberRow.arg(member2Loc).arg(date).arg(member2Name);
        fillerS -= memberRowHeight;
    }
    if (member3Name != "") {
        addMembers += additionalMemberRow.arg(member3Loc).arg(date).arg(member3Name);
        fillerS -= memberRowHeight;
    }
    if (member4Name != "") {
        addMembers += additionalMemberRow.arg(member4Loc).arg(date).arg(member4Name);
        fillerS -= memberRowHeight;
    }
    if (member5Name != "") {
        addMembers += additionalMemberRow.arg(member5Loc).arg(date).arg(member5Name);
        fillerS -= memberRowHeight;
    }

    query = "SELECT def_devname, def_devtype, def_kks, def_realdesc, def_repairdesc FROM trrworks "
            "LEFT JOIN defects ON trw_work = def_id "
            "WHERE trw_report = '%1' ORDER BY trw_order";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }
    workList.clear();
    while (db->nextRecord())
    {
        w.name = db->fetchValue(0).toString();
        w.type = db->fetchValue(1).toString();
        w.kks = db->fetchValue(2).toString();
        w.defect = db->fetchValue(3).toString();
        w.repair = db->fetchValue(4).toString();
        works += w.name + " " + w.type + ", " + w.kks + "; ";
        workList.append(w);
    }
    works.chop(2);

    if (workList.size() > 4) {
        works = "Согласно перечню оборудования к акту №%1 (Количество %2 шт.).";
        works = works.arg(docNum).arg(workList.size());
    }

    query = "SELECT sch_tdoc FROM schedule WHERE sch_type = '%1'";

    for (int i=0; i<workList.size(); i++)
    {
        prepQuery = query.arg(workList[i].type);
        if (!db->execQuery(prepQuery)) {
            db->showError(this);
            return result;
        }
        if (db->nextRecord())
            workList[i].techDoc = db->fetchValue(0).toString();
    }

    allRowsHeight = 0;
    for (int i=0; i<workList.size(); i++)
    {
        row = rowTmpl;
        work = workList[i].name + " " + workList[i].type + ", " + workList[i].kks;
        row.replace("$WORK$", work);
        row.replace("$TECHDOC$", workList[i].techDoc);
        tdocStr = "Обеспечение работоспособности в соответствии с " + workList[i].techDoc;
        row.replace("$DEFECT$", workList[i].defect);
        row.replace("$REPAIR$", workList[i].repair);

        strCount = 0;

        bounding = fm->boundingRect(workRect, Qt::TextWordWrap | Qt::AlignCenter, work);
        strCount = maxInt((bounding.height()+1)/20, strCount);
        bounding = fm->boundingRect(tdocRect, Qt::TextWordWrap | Qt::AlignCenter, tdocStr);
        strCount = maxInt((bounding.height()+1)/20, strCount);
        bounding = fm->boundingRect(defectRect, Qt::TextWordWrap | Qt::AlignCenter, workList[i].defect);
        strCount = maxInt((bounding.height()+1)/20, strCount);
        bounding = fm->boundingRect(repairRect, Qt::TextWordWrap | Qt::AlignCenter, workList[i].repair);
        strCount = maxInt((bounding.height()+1)/20, strCount);

        rowHeight = strCount *19;

        if (i+1 == workList.size()) {
            if (allRowsHeight + rowHeight > fillerS) {
                pages.append(rows);
                rows.clear();
                pages.append(row);

            } else {
                rows += row;
                pages.append(rows);
            }
        } else {
            if (allRowsHeight + rowHeight > fillerNS) {
                pages.append(rows);
                rows.clear();
                rows += row;
                allRowsHeight = rowHeight;
            } else {
                rows += row;
                allRowsHeight += rowHeight;
            }
        }
    }

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/ado-nosign.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/ado-nosign.html\n" + file.errorString());
        return result;
    }
    ts = new QTextStream(&file);
    pageTempNoSign = ts->readAll();
    delete ts;
    file.close();

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/ado-sign.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/ado-sign.html\n" + file.errorString());
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

        page.replace("$USN$", unitShortName);
        page.replace("$PAGECOUNT$", QString("%1").arg(pageCount));
        page.replace("$PAGE$", QString("%1").arg(i+1));
        page.replace("$SUBSYS$", subsystem);
        page.replace("$DOCNUM$", docNum);
        page.replace("$ROWS$", pages[i]);
        page.replace("$DATE$", date);
        page.replace("$WORKS$", works);

        if (i == pages.size()-1) {
            page.replace("$OWNERLOC$", ownerLoc);
            page.replace("$OWNERNAME$", ownerName);
            page.replace("$MEMBER1LOC$", member1Loc);
            page.replace("$MEMBER1NAME$", member1Name);
            page.replace("$AMEMBERS$", addMembers);
            page.replace("$REPAIRERLOC$", repairerLoc);
            page.replace("$REPAIRERNAME$", repairerName);
            page.replace("$CHIEFLOC$", chiefLoc);
            page.replace("$CHIEFNAME$", chiefName);
        }
        result.append(page);
    }

    return result;
}

QStringList TRReportsForm::makePO(QString reportId)
{
    const int fillerNS = 786;
    const int fillerS = 720;
    const int defaultHeight = 48;
    const QRect kksRect(0, 0, 108, 46);
    const QRect nameRect(0, 0, 175,46);
    const QRect typeRect(0, 0, 232, 46);
    int rowHeight, allRowsHeight, strCount, pageCount;
    QFontMetrics *fm = new QFontMetrics(QFont("Times New Roman", 12));
    QRect bounding;
    QStringList result;
    QList<QStringList> table;
    QStringList tblRow;
    QString unitShortName, subsystem, docNum, page, chiefLoc, chiefName, row, rows, pageTempSign, pageTempNoSign;
    QStringList pages;
    QFile file;
    QTextStream *ts;
    QString rowTmpl = "<tr class=\"porow\">\n"
                  "<td class=\"hc\">$NUM$</td>\n"
                  "<td colspan=\"2\" class=\"hc\">$KKS$</td>\n"
                  "<td class=\"hc\">$NAME$</td>\n"
                  "<td colspan=\"2\" class=\"hc\">$TYPE$</td>\n"
                  "<td colspan=\"2\" class=\"hc\">ТР</td>\n"
                  "</tr>";

    QString query = "SELECT unit_shortname, unit_subsys, trr_docnum FROM trreports "
                    "LEFT JOIN units ON trr_unit = unit_id "
                    "WHERE trr_id = '%1'";
    query = query.arg(reportId);

    if (db->execQuery(query)) {
        if (db->nextRecord()) {
            unitShortName = db->fetchValue(0).toString();
            subsystem = db->fetchValue(1).toString();
            docNum = db->fetchValue(2).toString();
        }
    } else {
        db->showError(this);
        return result;
    }

    query = "SELECT def_kks, def_devname, def_devtype FROM trrworks "
            "LEFT JOIN defects ON trw_work = def_id "
            "WHERE trw_report = '%1' ORDER BY trw_order";
    query = query.arg(reportId);
    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    table.clear();
    for (int i=1; db->nextRecord(); i++)
    {
        tblRow.clear();
        tblRow.append(QString("%1").arg(i));
        tblRow.append(db->fetchValue(0).toString());
        tblRow.append(db->fetchValue(1).toString());
        tblRow.append(db->fetchValue(2).toString());
        table.append(tblRow);
    }

    allRowsHeight = 0;
    for (int i=0; i<table.size(); i++)
    {
        row = rowTmpl;
        row.replace("$NUM$", table[i][0]);
        row.replace("$KKS$", table[i][1]);
        row.replace("$NAME$", table[i][2]);
        row.replace("$TYPE$", table[i][3]);
        strCount = 0;
        bounding = fm->boundingRect(kksRect, Qt::TextWordWrap | Qt::AlignCenter, table[i][1]);
        strCount = maxInt((bounding.height()+1)/20, strCount);
        bounding = fm->boundingRect(nameRect, Qt::TextWordWrap | Qt::AlignCenter, table[i][2]);
        strCount = maxInt((bounding.height()+1)/20, strCount);
        bounding = fm->boundingRect(typeRect, Qt::TextWordWrap | Qt::AlignCenter, table[i][3]);
        strCount = maxInt((bounding.height()+1)/20, strCount);
        if (strCount < 3) rowHeight = defaultHeight;
        else rowHeight = strCount * 19;
        if (i+1 == table.size()) {
            if (allRowsHeight + rowHeight > fillerS) {
                pages.append(rows);
                rows.clear();
                pages.append(row);

            } else {
                rows += row;
                pages.append(rows);
            }
        } else {
            if (allRowsHeight + rowHeight > fillerNS) {
                pages.append(rows);
                rows.clear();
                rows += row;
                allRowsHeight = rowHeight;
            } else {
                rows += row;
                allRowsHeight += rowHeight;
            }
        }
    }

    query = "SELECT sig_loc, sig_name, trs_role FROM trsigners "
            "LEFT JOIN signers ON trs_signer = sig_id "
            "WHERE trs_report = '%1'";
    query = query.arg(reportId);

    if (!db->execQuery(query)) {
        db->showError(this);
        return result;
    }

    while (db->nextRecord())
    {
        if (db->fetchValue(2).toInt() == TRSCHIEF) {
            chiefLoc = db->fetchValue(0).toString();
            chiefName = db->fetchValue(1).toString();
            break;
        }
    }

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/po-nosign.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/po-nosign.html\n" + file.errorString());
        return result;
    }
    ts = new QTextStream(&file);
    pageTempNoSign = ts->readAll();
    delete ts;
    file.close();

    file.setFileName(QApplication::applicationDirPath() + "/templates/reports/po-sign.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/reports/po-sign.html\n" + file.errorString());
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

        page.replace("$USN$", unitShortName);
        page.replace("$PAGECOUNT$", QString("%1").arg(pageCount));
        page.replace("$PAGE$", QString("%1").arg(i+1));
        page.replace("$SUBSYS$", subsystem);
        page.replace("$DOCNUM$", docNum);
        page.replace("$ROWS$", pages[i]);

        if (i == pages.size()-1) {
            page.replace("$CHIEFLOC$", chiefLoc);
            page.replace("$CHIEFNAME$", chiefName);
        }
        result.append(page);
    }

    return result;
}

void TRReportsForm::tableCellDoubleClicked(int row, int column)
{
    editButtonClicked();
}

