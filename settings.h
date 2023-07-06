#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{

public:
    struct ConnSettings {
        QString host;
        QString database;
        QString user;
        QString password;
    };

    static Settings* instance();
    ConnSettings getConnSettings() {return cs;};
    void setConnSettings(ConnSettings cs) {this->cs = cs;};
    void save();

private:
    static Settings *m_inst;
    Settings(){};
    ~Settings(){};
    void init();
    QSettings *s = nullptr;
    ConnSettings cs;
};

#endif // SETTINGS_H
