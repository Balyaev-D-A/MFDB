#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QWidget>


class Database
{
public:
    Database();
    ~Database();
    bool open(QString dbhost, QString dbname, QString dbuser, QString dbpwd);
    void close();
    QSqlError lastError();
    bool deployTables();
    bool isConnected();
    void showError(QWidget *sender);
    QString explodeFields(QStringList fields, unsigned char from);
    bool execQuery(const QString &query);
    QVariant fetchValue(int index);
    QVariant fetchValue(const QString &name);
    int affectedRows() const;
    bool firstRecord();
    bool nextRecord();
    bool seekRecord(int index, bool relative = false);
    bool startTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    QVariant lastInsertId() const;
    QVariant getVariable(QString varName);
    QList<QStringList> getResults();

private:
    QSqlDatabase db;
    QSqlQuery *pq;
    QSqlDatabase *pdb;

};

#endif // DATABASE_H
