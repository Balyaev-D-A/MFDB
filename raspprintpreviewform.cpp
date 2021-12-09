#include "raspprintpreviewform.h"
#include "ui_raspprintpreviewform.h"

#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

RaspPrintPreviewForm::RaspPrintPreviewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RaspPrintPreviewForm)
{
    ui->setupUi(this);

    connect(ui->saveButton, &QPushButton::clicked, this, &RaspPrintPreviewForm::saveToPDFClicked);

    pdfPrinter.setPageSize(QPrinter::A4);
    pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
    pdfPrinter.setOutputFileName("/home/user/print.pdf");
}

RaspPrintPreviewForm::~RaspPrintPreviewForm()
{
    delete ui;
}

void RaspPrintPreviewForm::setDatabase(Database *db)
{
    this->db = db;
}

void RaspPrintPreviewForm::showPreview(QStringList raspList)
{
    QString html;
    QString body = "";
    QString tabTemplate;
    QString table;
    QString query;
    QString executor;
    QString member;
    QString members;
    QString issuer;
    QString date;
    QString group;
    QList<QStringList> workList;
    QStringList work;
    QFile file;
    QTextStream *ts;

    file.setFileName(QApplication::applicationDirPath() + "/templates/rasp/table.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/rasp/table.html\n" + file.errorString());
        return;
    }
    ts = new QTextStream(&file);
    tabTemplate = ts->readAll();
    delete ts;
    file.close();

    file.setFileName(QApplication::applicationDirPath() + "/templates/rasp/rasp.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка!", "Невозможно открыть шаблон по пути: " +
                              QApplication::applicationDirPath() + "/templates/rasp/rasp.html\n" + file.errorString());
        return;
    }
    ts = new QTextStream(&file);
    html = ts->readAll();
    delete ts;
    file.close();

    foreach (QString rasp, raspList)
    {
        query = "select rasp_date, emp_name, emp_position, emp_group, iss_name, iss_loc from rasp "
                "left join employees on rasp_executor = emp_id "
                "left join issuers on rasp_issuer = iss_id where rasp_id = '" + rasp + "'";
        QMessageBox::information(this, "INFO", query);
        if (!db->pq->exec(query)) {
            db->showError(this);
            deleteLater();
            return;
        }

        db->pq->next();
        group = db->pq->value(3).toString().simplified();
        group.replace("1", "I гр.").replace("2", "II гр.").replace("3", "III гр.").replace("4", "IV гр.").replace("5", "V гр.");
        executor = db->pq->value(2).toString() + " " + db->pq->value(1).toString() + " " + group;
        date = db->pq->value(0).toString();
        issuer = db->pq->value(4).toString() + " " + db->pq->value(5).toString();

        table = tabTemplate;
        table.replace("%EXECUTOR%", executor).replace("%DATE%", date).replace("%ISSUER%", issuer);

        query = "select emp_name, emp_position, emp_group from rmembers left join employees on rm_emp = emp_id where rm_rasp = '" +
                rasp + "'";
        if (!db->pq->exec(query)) {
            db->showError(this);
            deleteLater();
            return;
        }
        members = "";
        table.replace("%N%", QString("%1").arg(db->pq->numRowsAffected()));
        while (db->pq->next())
        {
            group = db->pq->value(2).toString();
            group.replace("1", "I гр.").replace("2", "II гр.").replace("3", "III гр.").replace("4", "IV гр.").replace("5", "V гр.");
            member = db->pq->value(1).toString() + " " + db->pq->value(0).toString() + " " + group + ", ";
            members += member;
        }
        members.chop(2); //Убираем лишние ", " в конце.
        table.replace("%MEMBERS%", members);

        query = "select loc_location, sch_type, sch_kks, re_worktype from requipment "
                "left join schedule on re_equip = sch_id "
                "left join locations on sch_kks=loc_kks where re_rasp = '" + rasp + "'";
        if (!db->pq->exec(query)) {
            db->showError(this);
            deleteLater();
            return;
        }
        workList.clear();
        while (db->pq->next())
        {
            work.clear();
            for (int i=0; i<4; i++) work.append(db->pq->value(i).toString());
            workList.append(work);
        }

        body += table;
    }

    html.replace("%BODY%", body);
    ui->htmlView->setHtml(html);
    show();
}

void RaspPrintPreviewForm::saveToPDFClicked()
{
    ui->htmlView->page()->print(&pdfPrinter, [] (bool) {});
}
