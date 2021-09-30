#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>


class Database
{
public:
    Database();
    ~Database();
    bool open(QString dbhost, QString dbname, QString dbuser, QString dbpwd);
    QSqlError lastError();
    bool deployTables();
    bool isConnected();
    QString explodeFields(QStringList fields, unsigned char from);
    QSqlQuery *pq;
    QSqlDatabase *pdb;
private:
    QSqlDatabase db;

};

#endif // DATABASE_H
