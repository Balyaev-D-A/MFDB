#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QTranslator tr;
    tr.load(":/tr/qtbase_ru.qm");
    a.installTranslator(&tr);
    QTextCodec *utfcodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utfcodec);
    w.show();
    return a.exec();
}
