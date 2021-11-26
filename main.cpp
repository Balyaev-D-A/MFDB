#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QTranslator tr;
    tr.load(":/tr/qtbase_ru.qm");
    a.installTranslator(&tr);
    w.show();
    return a.exec();
}
