#ifndef STRINGLISTVALIDATOR_H
#define STRINGLISTVALIDATOR_H

#include <QValidator>
#include <QObject>

class StringListValidator : public QValidator
{
    Q_OBJECT
public:
    StringListValidator(QObject *parent = nullptr);
    void setList(QStringList list);

    virtual QValidator::State validate(QString &input, int &pos) const override;

private:
    QStringList valid;
};

#endif // STRINGLISTVALIDATOR_H
