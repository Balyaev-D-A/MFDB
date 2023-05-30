#include "jsonobjectbuilder.h"

JsonObjectBuilder::JsonObjectBuilder()
{

}

QString JsonObjectBuilder::startTabs()
{
    QString result;
    int l = level;
    while (l-- > 0) result += "\t";
    return result;
}

void JsonObjectBuilder::startObject()
{
    switch (lastOperation) {
    case OP_NONE:
    case OP_WRITE_NAME:
        jsonText += "{\n";
        break;
    default:
        jsonText += startTabs() + "{\n";
    }
    level++;
}

void JsonObjectBuilder::startArray()
{
    switch (lastOperation) {
    case OP_NONE:
    case OP_WRITE_NAME:
        jsonText += "[\n";
        break;
    default:
        jsonText += startTabs() + "[\n";
    }

    level++;
}
void JsonObjectBuilder::endObject()
{
    level--;
    jsonText += startTabs() + "}\n";
}

void JsonObjectBuilder::endArray()
{
    level--;
    jsonText += startTabs() + "]\n";
}
