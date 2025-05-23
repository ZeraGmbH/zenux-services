#ifndef JSONSTRUCTURELOADER_H
#define JSONSTRUCTURELOADER_H

#include "supportedsources.h"
#include "sourceproperties.h"
#include <QString>
#include <QJsonObject>

class JsonStructureLoader
{
public:
    static QJsonObject loadJsonStructureFromFile(QString fileName);
    static QJsonObject loadJsonDefaultStructure(SupportedSourceTypes type);
    static QJsonObject loadJsonStructure(SupportedSourceTypes type, QString deviceName, QString deviceVersion);
    static QJsonObject loadJsonStructure(SourceProperties properties);
};

#endif // JSONSTRUCTURELOADER_H
