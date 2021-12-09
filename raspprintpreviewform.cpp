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
    pdfPrinter.setPageSize(QPrinter::A4);
}

RaspPrintPreviewForm::~RaspPrintPreviewForm()
{
    delete ui;
}

void RaspPrintPreviewForm::showPreview(QStringList raspList)
{
    QString html;
    QString body;
    QString tabTemplate;
    QString table;
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
    body = tabTemplate + tabTemplate + tabTemplate + tabTemplate;
    html.replace("%BODY%", body);
    ui->htmlView->setHtml(html);
    show();
}
