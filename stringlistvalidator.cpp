#include "stringlistvalidator.h"

StringListValidator::StringListValidator(QObject *parent)
    :QValidator(parent)
{

}

void StringListValidator::setList(QStringList list)
{
    valid = list;
}

QValidator::State StringListValidator::validate(QString &input, int &pos) const
{
    for (int i=0; i<valid.count(); i++)
    {
        if (valid[i].indexOf(input, 0, Qt::CaseInsensitive) == 0) return QValidator::Acceptable;
    }
    return QValidator::Invalid;
}
