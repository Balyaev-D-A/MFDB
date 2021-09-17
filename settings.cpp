#include <QApplication>
#include "settings.h"

Settings::Settings()
{
    s = new QSettings(QApplication::applicationDirPath() + "settings.ini", QSettings::IniFormat);
}
