#ifndef CSVREADER_H
#define CSVREADER_H

#include <QString>
#include <QList>
#include <QFile>
#include <QTextStream>

typedef QList<QString> CSVRow;

class CSVReader
{
public:
    CSVReader();
    bool openFile(QString filename);
    CSVRow readNextRow();
    void closeFile();
    bool atEnd();
private:
    QFile file;
    QTextStream *ts;
    bool isLastValue;
    QString readValue();

};

#endif // CSVREADER_H
