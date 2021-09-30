#include <QStringList>
#include "database.h"

Database::Database()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    pdb = &db;
    pq = new QSqlQuery(db);
}

Database::~Database()
{
    delete pq;
}

bool Database::open(QString dbhost, QString dbname, QString dbuser, QString dbpwd)
{
    pdb->setHostName(dbhost);
    pdb->setDatabaseName(dbname);
    pdb->setUserName(dbuser);
    pdb->setPassword(dbpwd);
    return pdb->open();
}

QSqlError Database::lastError()
{
    return pdb->lastError();
}

QString Database::explodeFields(QStringList fields, unsigned char from)
{
    QString result = "";
    for (int i=from; i<fields.count(); i++ ) {
        if (i>from) result += ", ";
        result += fields.at(i);
    }
    return result;
}

bool Database::deployTables()
{

/////////////////////////////////////////////////////////// Справочники //////////////////////////////////////////////////

    if (!pq->exec("create table if not exists employees (emp_id serial primary key, emp_num smallint unique, emp_name varchar(100), "
                  "emp_position varchar(255), emp_group smallint, emp_tld smallint unique, emp_admin bool default 'false', "
                  "emp_metrolog bool default 'false', emp_hidden bool default 'false')")) return false;

    if (!pq->exec("create table if not exists units (unit_id serial primary key, unit_name varchar(20))")) return false;

    if (!pq->exec("create table if not exists schedule (sch_id serial primary key, sch_unit integer references units(unit_id), "
                  "sch_kks varchar(50), sch_name varchar(255), sch_type varchar(50), sch_tdoc varchar(255), sch_date varchar(10), "
                  "sch_reportdate varchar(10), sch_worktype char(2), sch_hours decimal(5,2), sch_executor char(10), sch_state integer default 0)")) return false;

//////////////////////////////////////////////////////// Работы /////////////////////////////////////////////////////////
    if (!pq->exec("create table if not exists rasp (rasp_id serial primary key, rasp_num char(6) default '', rasp_date char(10), rasp_btime char(5),"
                  " rasp_etime char(5), rasp_issuing char(100), rasp_executor integer references employees(emp_id))")) return false;

    if (!pq->exec("create table if not exists rmembers (rm_id serial primary key, rm_rasp integer references rasp(rasp_id), "
                  "rm_emp integer references employees(emp_id))")) return false;

    if (!pq->exec("create table if not exists requipment (re_id serial primary key, re_rasp integer references rasp(rasp_id), "
                  "re_equip integer references schedule(sch_id), re_worktype char(10))")) return false;

    return true;
}
