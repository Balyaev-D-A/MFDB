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

bool Database::deployTables()
{

/////////////////////////////////////////////////////////// Справочники //////////////////////////////////////////////////

    if (!pq->exec("create table if not exists employees (emp_id smallint unsigned not null auto_increment, "
                  "emp_num smallint unsigned unique, emp_name varchar(100), "
                  "emp_position varchar(255), emp_group tinyint(1), emp_tld smallint unsigned unique, emp_admin bool, primary key(emp_id))")) return false;

    if (!pq->exec("create table if not exists devices (dev_id unsigned smallint not null auto_increment, dev_type varchar(50) not null unique, "
                  "dev_name varchar(255), dev_docid varchar(100), dev_to decimal(5,2), "
                  "dev_tr decimal(5,2) dev_kr(5,2), primary key(dev_id))")) return false;

    if (!pq->exec("create table if not exists signers (sign_id unsigned tinyint not null auto_increment, sign_name char(100),"
                  "sign_pos char(100), primary key(sign_id))")) return false;

    if (!pq->exec("create table if not exists units (unit_id unsigned tinyint not null auto_increment, unit_name varchar(20),"
                  "unit_sign unsigned tinyint, primary key(unit_id))")) return false;

    if (!pq->exec("create table if not exists executors (exec_id unsigned tinyint not null auto_increment, exec_name varchar(20),"
                  "primary key(exec_id))")) return false;

    if (!pq->exec("create table if not exists equipment (eq_id unsigned mediumin not null auto_increment, eq_pos varchar(20) not null unique,"
                  "eq_type unsigned smallint not null, eq_invno varchar(20) not null unique, eq_unit unsigned tinyint not null, "
                  "eq_room varchar(20), eq_sign unsigned tinyint, primary key(eq_id))")) return false;

    if (!pq->exec("create table if not exists worktype (wt_id unsigned tinyint not null auto_increment, wt_fname varchar(255), wt_sname varchar(10) not null, "
                  "primary key(exec_id))")) return false;

    //////////////////////////////////////////////////////// Работы /////////////////////////////////////////////////////////

    if (!pq->exec("create table if not exists tochart (to_id unsigned mediumint not null auto_increment, to_equip unsigned mediumint, to_date date, to_worktype unsigned tinyint,"
                  "to_executor unsigned tinyint, primary key(to_id))")) return false;

    //if (!pq->exec("create table if not exists machart (to_equip unsigned mediumint not null, to_date date, to_worktype unsigned tinyint,"
    //              "to_executor unsigned tinyint, primary key(to_equip))")) return false;

    if (!pq->exec("create table if not exists ordermembers (om_id unsigned mediumint not null auto_increment, om_order unsigned mediumint not null, "
                  "om_employee unsigned smallint not null, om_bdate datetime, om_edate datetime, om_master bool, primary key(om_id))")) return false;

    if (!pq->exec("create table if not exists orderworks (ow_id unsigned mediumint not null auto_increment, ow_order unsigned mediumint not null, "
                  "ow_date date not null, ow_equipment unsigned mediumint not null, ow_worktype unsigned tinyint not null, primary key(om_id))")) return false;

    if (!pq->exec("create table if not exists orders (o_id unsigned mediumint not null auto_increment, o_number varchar(10) not null, "
                  "o_bdate date not null, o_edate date not null, o_doseorder unsigned mediumint, o_oneday bool, o_status tinyint(1), primary key(om_id))")) return false;

    return true;
}

