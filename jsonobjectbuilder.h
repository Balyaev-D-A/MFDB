#ifndef JSONOBJECTBUILDER_H
#define JSONOBJECTBUILDER_H

#include <QString>

class JsonObjectBuilder
{
public:
    enum OPERATION
    {
        OP_OPEN_OBJECT,
        OP_CLOSE_OBJECT,
        OP_OPEN_ARRAY,
        OP_CLOSE_ARRAY,
        OP_WRITE_NAME,
        OP_WRITE_VALUE,
        OP_NONE
    };
    JsonObjectBuilder();
    void writeName(QString name);
    void writeValue(int value);
    void writeValue(float value);
    void writeValue(QString value);
    void startObject();
    void startArray();
    void endObject();
    void endArray();

private:
    int level = 1;
    OPERATION lastOperation = OP_NONE;
    QString jsonText;
    QString startTabs();
};

#endif // JSONOBJECTBUILDER_H
