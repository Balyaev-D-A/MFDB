#include <QApplication>
#include "settings.h"

Settings* Settings::m_inst = 0;

Settings* Settings::instance()
{
    if (!m_inst) {
        m_inst = new Settings();
        m_inst->init();
    }
    return m_inst;
}

void Settings::init()
{
    s = new QSettings(QApplication::applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    s->beginGroup("connection");
    cs.host = s->value("host").toString();
    cs.database = s->value("database").toString();
    cs.user = s->value("user").toString();
    cs.password = s->value("password").toString();
    s->endGroup();
}

void Settings::save()
{
    s->beginGroup("connection");
    s->setValue("host", cs.host);
    s->setValue("database", cs.database);
    s->setValue("user", cs.user);
    s->setValue("password", cs.password);
    s->endGroup();
    s->sync();
}
