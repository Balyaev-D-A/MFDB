#include <QStringList>
#include <QMessageBox>
#include <QVariant>
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
                  "sch_kks varchar(50), sch_invno varchar(20), sch_name varchar(255), sch_type varchar(50), sch_tdoc varchar(255), sch_date varchar(10), "
                  "sch_reportdate varchar(10), sch_worktype char(2), sch_hours decimal(5,2), sch_executor char(10), sch_done bool default 'false', "
                  "sch_note varchar(1000))")) return false;

    if (!pq->exec("create table if not exists locations (loc_kks varchar(50) primary key, loc_location varchar(100))")) return false;

    if (!pq->exec("create table if not exists issuers (iss_id serial primary key, iss_name varchar(50), iss_loc varchar(20), "
                  "iss_default bool default false)")) return false;

    if (!pq->exec("create table if not exists materials (mat_id serial primary key, mat_name varchar(150), mat_doc varchar(150), "
                  "mat_measure varchar(10))")) return false;

    if (!pq->exec("create table if not exists normativmat (nm_id serial primary key, nm_dev varchar(50), nm_worktype char(2), "
                  "nm_material integer references materials(mat_id), nm_count decimal(5,2))")) return false;

    if (!pq->exec("create table if not exists normativwork (nw_id serial primary key, nw_dev varchar(50), nw_worktype char(2), "
                  "nw_oesn varchar(255), nw_work decimal(5,2))")) return false;

    if (!pq->exec("create table if not exists normativactions (na_id serial primary key, na_dev varchar(50), na_worktype char(2), "
                  "na_actions text)")) return false;

//////////////////////////////////////////////////////// Работы /////////////////////////////////////////////////////////
    if (!pq->exec("create table if not exists rasp (rasp_id serial primary key, rasp_num char(6), rasp_date char(10), rasp_btime char(5), "
                  "rasp_etime char(5), rasp_issuer integer references issuers(iss_id), rasp_executor integer references employees(emp_id), "
                  "rasp_completed bool default 'false')")) return false;

    if (!pq->exec("create table if not exists rmembers (rm_id serial primary key, rm_rasp integer references rasp(rasp_id) on delete cascade, "
                  "rm_emp integer references employees(emp_id))")) return false;

    if (!pq->exec("create table if not exists requipment (re_id serial primary key, re_rasp integer references rasp(rasp_id) on delete cascade, "
                  "re_equip integer references schedule(sch_id), re_worktype char(16))")) return false;

    return true;
}

void Database::showError(QWidget *sender)
{
    QMessageBox::critical(sender, "Ошибка выполнения запроса!", pq->lastError().text());
}

bool Database::execQuery(const QString &query)
{
    return pq->exec(query);
}

QVariant Database::fetchValue(int index)
{
    return pq->value(index);
}

QVariant Database::fetchValue(const QString &name)
{
    return pq->value(name);
}

int Database::affectedRows() const
{
    return pq->numRowsAffected();
}

bool Database::nextRecord()
{
    return pq->next();
}

bool Database::startTransaction()
{
    return pdb->transaction();
}

bool Database::commitTransaction()
{
    return pdb->commit();
}

bool Database::rollbackTransaction()
{
    return pdb->rollback();
}

QVariant Database::lastInsertId() const
{
    return pq->lastInsertId();
}

bool Database::seekRecord(int index, bool relative)
{
    return pq->seek(index, relative);
}

void Database::close()
{
    pdb->close();
}

bool Database::firstRecord()
{
    return pq->first();
}
