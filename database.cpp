#include <QStringList>
#include <QMessageBox>
#include <QVariant>
#include "database.h"

Database *Database::m_inst = 0;

Database* Database::instance()
{
    if (!m_inst) {
        m_inst = new Database();
        m_inst->init();
    }
    return m_inst;

}

void Database::init()
{
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_pdb = &m_db;
    m_pq = new QSqlQuery(m_db);
}

Database::~Database()
{
    delete m_pq;
}

bool Database::open(QString dbhost, QString dbname, QString dbuser, QString dbpwd)
{
    m_pdb->setHostName(dbhost);
    m_pdb->setDatabaseName(dbname);
    m_pdb->setUserName(dbuser);
    m_pdb->setPassword(dbpwd);
    return m_pdb->open();
}

QSqlError Database::lastError()
{
    return m_pdb->lastError();
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

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS departments (dep_id SERIAL PRIMARY KEY, dep_name VARCHAR(50)")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS locations (loc_id SERIAL PRIMARY KEY, loc_name VARCHAR(100)), "
                    "loc_department INTEGER REFERENCES departments(dep_id), loc_ownerphone VARCHAR(10)")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS devicetypes (dt_id SERIAL PRIMARY KEY, dt_name VARCHAR(50), dt_description VARCHAR(255), "
                    "dt_techdoc VARCHAR(255))")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS devices(dev_id SERIAL PRIMARY KEY, dev_type INTEGER REFERENCES devicetypes(dt_id), "
                    "dev_kks VARCHAR(50), dev_invnumber VARCHAR(20), dev_unit INTEGER REFERENCES units(unit_id), "
                    "dev_location INTEGER REFERENCES locations(loc_id), dev_executor VARCHAR(10)")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS employees (emp_id SERIAL PRIMARY KEY, emp_num SMALLINT UNIQUE, emp_name VARCHAR(100), "
                    "emp_position VARCHAR(255), emp_group SMALLINT, emp_tld SMALLINT UNIQUE, emp_admin BOOL DEFAULT 'FALSE', "
                    "emp_metrolog BOOL DEFAULT 'FALSE', emp_hidden BOOL DEFAULT 'FALSE')")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS units (unit_id SERIAL PRIMARY KEY, unit_name VARCHAR(20), unit_shortname VARCHAR(10), "
                    " unit_subsys VARCHAR(20), unit_schednum VARCHAR(50))")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS worktypes (wt_id SERIAL PRIMARY KEY, wt_name VARCHAR(10)")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS statuses (st_id SERIAL PRIMARY KEY, st_name VARCHAR(20), st_style VARCHAR(300), "
                    "st_brush SMALLINT")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS schedule (sch_id SERIAL PRIMARY KEY, sch_device INTEGER REFERENCES devices(dev_id), "
                    "sch_worktype INTEGER REFERENCES worktypes(wt_id), sch_state INTEGER")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS issuers (iss_id SERIAL PRIMARY KEY, iss_name VARCHAR(50), iss_loc VARCHAR(50), "
                    "iss_default BOOL DEFAULT FALSE, iss_hidden BOOL DEFAULT FALSE)")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS materials (mat_id SERIAL PRIMARY KEY, mat_name VARCHAR(300), "
                    "mat_measure VARCHAR(10), mat_consumable BOOL DEFAULT 'FALSE')")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS materialbatches (mb_id SERIAL PRIMARY KEY, mb_actnumber VARCHAR(10), mb_actdate DATE")) return false;

    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS normativmat (nm_id SERIAL PRIMARY KEY, nm_devtype INTEGER REFERENCES devicetypes(dt_id), "
                    "nm_worktype INTEGER REFERENCES worktypes(wt_id), nm_material INTEGER REFERENCES materials(mat_id), "
                    "nm_count DECIMAL(6,4))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS normativwork (nw_id SERIAL PRIMARY KEY, nw_dev VARCHAR(50), nw_worktype CHAR(2), "
//                  "nw_oesn VARCHAR(255), nw_ktd VARCHAR(255), nw_ktdshort VARCHAR(100), nw_work DECIMAL(5,2))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS normativactions (na_id SERIAL PRIMARY KEY, na_dev VARCHAR(50), na_worktype CHAR(2), "
//                  "na_actions TEXT)")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS defects (def_id serial primary key, def_unit INTEGER REFERENCES units(unit_id), def_quarter smallint, def_num varchar(10), "
//                  "def_devname VARCHAR(255), def_devtype varchar(50), def_kks varchar(50), "
//                  "def_journaldesc text, def_realdesc text, def_stage integer, def_repairdesc text, def_actionsdesc text, def_begdate char(10), def_enddate char(10), "
//                  "def_rasp CHAR(6))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS defrealdescs (drd_id SERIAL PRIMARY KEY, drd_dev VARCHAR(50), drd_stage INTEGER, drd_desc TEXT)")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS defrepairdescs (drp_id SERIAL PRIMARY KEY, drp_realdefdesc INTEGER REFERENCES defrealdescs(drd_id), "
//                  "drp_repairdesc TEXT)")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS defadditionalmats (dam_id SERIAL PRIMARY KEY, dam_defect INTEGER REFERENCES defects(def_id), "
//                  "dam_material INTEGER REFERENCES materials (mat_id), dam_order INTEGER DEFAULT 0, dam_count DECIMAL(6,3), dam_oesn DECIMAL(6,3))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS kaprepairs (kr_id SERIAL PRIMARY KEY, kr_sched INTEGER REFERENCES schedule(sch_id) UNIQUE, "
//                  "kr_actions TEXT, kr_begdate CHAR(10), kr_enddate CHAR(10))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS kradditionalmats (kam_id SERIAL PRIMARY KEY, kam_kr INTEGER REFERENCES kaprepairs(kr_id), "
//                  "kam_material INTEGER REFERENCES materials (mat_id), kam_order INTEGER DEFAULT 0, kam_count DECIMAL(6,3), kam_oesn DECIMAL(6,3))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS receipts (rec_id serial primary key, rec_date CHAR(10), "
//                  "rec_description VARCHAR(255))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS recmaterials (rcm_id SERIAL PRIMARY KEY, rcm_rec INTEGER REFERENCES receipts(rec_id), "
//                  "rcm_material INTEGER REFERENCES materials (mat_id), rcm_count DECIMAL(5,2))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS signers (sig_id SERIAL PRIMARY KEY, sig_name VARCHAR(50), sig_loc VARCHAR(50), "
//                  "sig_hidden BOOL DEFAULT FALSE)")) return false;

      if (!m_pq->exec("CREATE TABLE IF NOT EXISTS variables (var_id SERIAL PRIMARY KEY, var_name VARCHAR(50) UNIQUE, var_value TEXT)")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS krreports (krr_id SERIAL PRIMARY KEY, krr_desc VARCHAR(200), krr_unit INTEGER REFERENCES units(unit_id), "
//                  "krr_planbeg CHAR(10), krr_planend CHAR(10), krr_date CHAR(10), krr_docnum VARCHAR(10))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS krsigners (krs_id SERIAL PRIMARY KEY, krs_report INTEGER REFERENCES krreports(krr_id) ON DELETE CASCADE, "
//                  "krs_signer INTEGER REFERENCES signers(sig_id) ON DELETE RESTRICT, krs_role INTEGER)")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS krrworks (krw_id SERIAL PRIMARY KEY, krw_work INTEGER REFERENCES kaprepairs(kr_id) ON DELETE RESTRICT, "
//                  "krw_report INTEGER REFERENCES krreports(krr_id) ON DELETE CASCADE, krw_order INTEGER DEFAULT 0)")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS trreports (trr_id SERIAL PRIMARY KEY, trr_desc VARCHAR(200), trr_unit INTEGER REFERENCES units(unit_id), "
//                  "trr_planbeg CHAR(10), trr_planend CHAR(10), trr_date CHAR(10), trr_docnum VARCHAR(10))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS trsigners (trs_id SERIAL PRIMARY KEY, trs_report INTEGER REFERENCES trreports(trr_id) ON DELETE CASCADE, "
//                  "trs_signer INTEGER REFERENCES signers(sig_id) ON DELETE RESTRICT, trs_role INTEGER)")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS trrworks (trw_id SERIAL PRIMARY KEY, trw_work INTEGER REFERENCES defects(def_id) ON DELETE RESTRICT, "
//                  "trw_report INTEGER REFERENCES trreports(trr_id) ON DELETE CASCADE)")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS ktd (ktd_dev VARCHAR(50) PRIMARY KEY, ktd_doc VARCHAR(50))")) return false;

////////////////////////////////////////////////////////// Работы /////////////////////////////////////////////////////////
//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS rasp (rasp_id SERIAL PRIMARY KEY, rasp_num CHAR(6), rasp_date CHAR(10), rasp_btime CHAR(5), "
//                  "rasp_etime CHAR(5), rasp_issuer INTEGER REFERENCES issuers(iss_id), rasp_executor INTEGER REFERENCES employees(emp_id), "
//                  "rasp_zkd BOOL, rasp_unit CHAR(20), rasp_completed BOOL DEFAULT 'FALSE')")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS rmembers (rm_id SERIAL PRIMARY KEY, rm_rasp INTEGER REFERENCES rasp(rasp_id) ON DELETE CASCADE, "
//                  "rm_emp INTEGER REFERENCES employees(emp_id))")) return false;

//    if (!m_pq->exec("CREATE TABLE IF NOT EXISTS requipment (re_id SERIAL PRIMARY KEY, re_rasp INTEGER REFERENCES rasp(rasp_id) ON DELETE CASCADE, "
//                  "re_equip INTEGER REFERENCES schedule(sch_id), re_worktype CHAR(16))")) return false;

    return true;
}

void Database::showError(QWidget *sender)
{
    QMessageBox::critical(sender, "Ошибка выполнения запроса!", m_pq->lastError().text());
}

bool Database::execQuery(const QString &query)
{
    return m_pq->exec(query);
}

QVariant Database::fetchValue(int index)
{
    return m_pq->value(index);
}

QVariant Database::fetchValue(const QString &name)
{
    return m_pq->value(name);
}

int Database::affectedRows() const
{
    return m_pq->numRowsAffected();
}

bool Database::nextRecord()
{
    return m_pq->next();
}

bool Database::startTransaction()
{
    return m_pdb->transaction();
}

bool Database::commitTransaction()
{
    return m_pdb->commit();
}

bool Database::rollbackTransaction()
{
    return m_pdb->rollback();
}

QVariant Database::lastInsertId() const
{
    return m_pq->lastInsertId();
}

bool Database::seekRecord(int index, bool relative)
{
    return m_pq->seek(index, relative);
}

void Database::close()
{
    m_pdb->close();
}

bool Database::firstRecord()
{
    return m_pq->first();
}

QVariant Database::getVariable(QString varName)
{
    QVariant result;
    QString query = "SELECT var_value FROM variables WHERE var_name = '%1'";
    query = query.arg(varName);
    if (!execQuery(query))
        return result;
    if (nextRecord()) {
        result = fetchValue(0);
    }
    return result;
}

Database::DbResults Database::getResults()
{
    QVariantList resultRow;
    Database::DbResults result;

    while (nextRecord())
    {
        resultRow.clear();
        for (int i=0; i<m_pq->record().count(); i++)
            resultRow.append(fetchValue(i));
        result.append(resultRow);
    }
    return result;
}
