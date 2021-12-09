#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QWidget>


class Database
{
public:
    Database();
    ~Database();
    bool open(QString dbhost, QString dbname, QString dbuser, QString dbpwd);
    QSqlError lastError();
    bool deployTables();
    bool isConnected();
    void showError(QWidget *sender);
    QString explodeFields(QStringList fields, unsigned char from);
    bool execQuery(const QString &query);
    QVariant fetchValue(int index);
    QVariant fetchValue(const QString &name);
    int affectedRows() const;
    bool nextRecord();
    bool seekRecord(int index, bool relative = false);
    bool startTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    QVariant lastInsertId() const;
    QSqlQuery *pq;
    QSqlDatabase *pdb;

private:
    QSqlDatabase db;

};

#endif // DATABASE_H
