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

    if (!pq->exec("CREATE TABLE IF NOT EXISTS employees (emp_id SERIAL PRIMARY KEY, emp_num SMALLINT UNIQUE, emp_name VARCHAR(100), "
                  "emp_position VARCHAR(255), emp_group SMALLINT, emp_tld SMALLINT UNIQUE, emp_admin BOOL DEFAULT 'FALSE', "
                  "emp_metrolog BOOL DEFAULT 'FALSE', emp_hidden BOOL DEFAULT 'FALSE')")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS units (unit_id SERIAL PRIMARY KEY, unit_name VARCHAR(20), unit_shortname VARCHAR(10), "
                  " unit_subsys VARCHAR(20), unit_schednum VARCHAR(50))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS schedule (sch_id SERIAL PRIMARY KEY, sch_unit INTEGER REFERENCES units(unit_id), "
                  "sch_kks VARCHAR(50), sch_invno VARCHAR(20), sch_name VARCHAR(255), sch_type VARCHAR(50), sch_tdoc VARCHAR(255), sch_date VARCHAR(10), "
                  "sch_reportdate VARCHAR(10), sch_worktype CHAR(2), sch_hours DECIMAL(5,2), sch_executor CHAR(10), sch_done BOOL DEFAULT 'FALSE', "
                  "sch_note VARCHAR(1000))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS locations (loc_kks VARCHAR(50) PRIMARY KEY, loc_location VARCHAR(100))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS issuers (iss_id SERIAL PRIMARY KEY, iss_name VARCHAR(50), iss_loc VARCHAR(50), "
                  "iss_default BOOL DEFAULT FALSE)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS materials (mat_id SERIAL PRIMARY KEY, mat_name VARCHAR(150), mat_doc VARCHAR(150), "
                  "mat_measure VARCHAR(10))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS normativmat (nm_id SERIAL PRIMARY KEY, nm_dev VARCHAR(50), nm_worktype CHAR(2), "
                  "nm_material INTEGER REFERENCES materials(mat_id), nm_count DECIMAL(6,3))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS normativwork (nw_id SERIAL PRIMARY KEY, nw_dev VARCHAR(50), nw_worktype CHAR(2), "
                  "nw_oesn VARCHAR(255), nw_work DECIMAL(5,2))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS normativactions (na_id SERIAL PRIMARY KEY, na_dev VARCHAR(50), na_worktype CHAR(2), "
                  "na_actions TEXT)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS defects (def_id serial primary key, def_unit INTEGER REFERENCES units(unit_id), def_quarter smallint, def_num varchar(10), "
                  "def_devname VARCHAR(255), def_devtype varchar(50), def_kks varchar(50), "
                  "def_journaldesc text, def_realdesc text, def_stage integer, def_repairdesc text, def_actionsdesc text, def_begdate char(10), def_enddate char(10), "
                  "def_rasp CHAR(6))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS defrealdescs (drd_id SERIAL PRIMARY KEY, drd_dev VARCHAR(50), drd_stage INTEGER, drd_desc TEXT)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS defrepairdescs (drp_id SERIAL PRIMARY KEY, drp_realdefdesc INTEGER REFERENCES defrealdescs(drd_id), "
                  "drp_repairdesc TEXT)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS defadditionalmats (dam_id SERIAL PRIMARY KEY, dam_defect INTEGER REFERENCES defects(def_id), "
                  "dam_material INTEGER REFERENCES materials (mat_id), dam_count DECIMAL(6,3))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS kaprepairs (kr_id SERIAL PRIMARY KEY, kr_sched INTEGER REFERENCES schedule(sch_id) UNIQUE, "
                  "kr_begdate CHAR(10), kr_enddate CHAR(10))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS kradditionalmats (kam_id SERIAL PRIMARY KEY, kam_kr INTEGER REFERENCES kaprepairs(kr_id), "
                  "kam_material INTEGER REFERENCES materials (mat_id), kam_count DECIMAL(6,3))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS receipts (rec_id serial primary key, rec_date CHAR(10), "
                  "rec_description VARCHAR(255))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS recmaterials (rcm_id SERIAL PRIMARY KEY, rcm_rec INTEGER REFERENCES receipts(rec_id), "
                  "rcm_material INTEGER REFERENCES materials (mat_id), rcm_count DECIMAL(5,2))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS signers (sig_id SERIAL PRIMARY KEY, sig_name VARCHAR(50), sig_loc VARCHAR(50), "
                  "sig_hidden BOOL DEFAULT FALSE)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS variables (var_id SERIAL PRIMARY KEY, var_name VARCHAR(50) UNIQUE, var_value TEXT)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS krreports (krr_id SERIAL PRIMARY KEY, krr_desc VARCHAR(200), krr_unit INTEGER REFERENCES units(unit_id), "
                  "krr_planbeg CHAR(10), krr_planend CHAR(10), krr_date CHAR(10), krr_docnum VARCHAR(10))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS krsigners (krs_id SERIAL PRIMARY KEY, krs_report INTEGER REFERENCES krreports(krr_id) ON DELETE CASCADE, "
                  "krs_signer INTEGER REFERENCES signers(sig_id) ON DELETE RESTRICT, krs_role INTEGER)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS krrworks (krw_id SERIAL PRIMARY KEY, krw_work INTEGER REFERENCES kaprepairs(kr_id) ON DELETE RESTRICT, "
                  "krw_report INTEGER REFERENCES krreports(krr_id) ON DELETE CASCADE)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS trreports (trr_id SERIAL PRIMARY KEY, trr_desc VARCHAR(200), trr_unit INTEGER REFERENCES units(unit_id), "
                  "trr_planbeg CHAR(10), trr_planend CHAR(10), trr_date CHAR(10), trr_docnum VARCHAR(10))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS trsigners (trs_id SERIAL PRIMARY KEY, trs_report INTEGER REFERENCES trreports(trr_id) ON DELETE CASCADE, "
                  "trs_signer INTEGER REFERENCES signers(sig_id) ON DELETE RESTRICT, trs_role INTEGER)")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS trrworks (trw_id SERIAL PRIMARY KEY, trw_work INTEGER REFERENCES defects(def_id) ON DELETE RESTRICT, "
                  "trw_report INTEGER REFERENCES trreports(trr_id) ON DELETE CASCADE)")) return false;

//////////////////////////////////////////////////////// Работы /////////////////////////////////////////////////////////
    if (!pq->exec("CREATE TABLE IF NOT EXISTS rasp (rasp_id SERIAL PRIMARY KEY, rasp_num CHAR(6), rasp_date CHAR(10), rasp_btime CHAR(5), "
                  "rasp_etime CHAR(5), rasp_issuer INTEGER REFERENCES issuers(iss_id), rasp_executor INTEGER REFERENCES employees(emp_id), "
                  "rasp_completed BOOL DEFAULT 'FALSE')")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS rmembers (rm_id SERIAL PRIMARY KEY, rm_rasp INTEGER REFERENCES rasp(rasp_id) ON DELETE CASCADE, "
                  "rm_emp INTEGER REFERENCES employees(emp_id))")) return false;

    if (!pq->exec("CREATE TABLE IF NOT EXISTS requipment (re_id SERIAL PRIMARY KEY, re_rasp INTEGER REFERENCES rasp(rasp_id) ON DELETE CASCADE, "
                  "re_equip INTEGER REFERENCES schedule(sch_id), re_worktype CHAR(16))")) return false;

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

QVariant Database::getVariable(QString varName)
{
    QVariant result = NULL;
    QString query = "SELECT var_value FROM variables WHERE var_name = '%1'";
    query = query.arg(varName);
    if (!execQuery(query))
        return result;
    if (nextRecord()) {
        result = fetchValue(0);
    }
    return result;
}
