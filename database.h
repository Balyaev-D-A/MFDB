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
    QSqlQuery *pq;
    QSqlDatabase *pdb;
private:
    QSqlDatabase db;
   // QSqlQuery *pq;

};

#endif // DATABASE_H
