#include "csvreader.h"
#include <QTextStream>

CSVReader::CSVReader()
{

}

bool CSVReader::openFile(QString filename)
{
    file.setFileName(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ts = new QTextStream(&file);
        return true;
    }
    else
        return false;
}

void CSVReader::closeFile()
{
    delete ts;
    file.close();
}

CSVRow CSVReader::readNextRow()
{
    CSVRow result;
    isLastValue = false;
    while (!isLastValue)
    {
        result.append(readValue());
    }
    return result;
}

QString CSVReader::readValue()
{
    QString value = "";
    QString ch = "";

    ch = ts->read(1);
    while (ch != "," && ch != "\n" && !ts->atEnd())
    {
        if (ch == "\"") {
            while (1)
            {
                ch = ts->read(1);
                if (ch == "\"") {
                    ch = ts->read(1);
                    if (ch != "\"") break;
                }
                value += ch;
            }
        }
        else {
            value += ch;
            ch = ts->read(1);
        }
    }
    if (ch == "\n" || ts->atEnd()) isLastValue = true;
    return value.simplified();
}

bool CSVReader::atEnd()
{
    return ts->atEnd();
}
