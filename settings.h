#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{
public:
    Settings();
private:
    QSettings *s;
};

#endif // SETTINGS_H
