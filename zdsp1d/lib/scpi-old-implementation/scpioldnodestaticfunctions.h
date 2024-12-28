#ifndef SCPIOLDNODESTATICFUNCTIONS_H
#define SCPIOLDNODESTATICFUNCTIONS_H

#include "scpi-zdsp.h"
#include <QStringList>
#include <QDomDocument>

class ScpiOldNodeStaticFunctions
{
public:
    static bool isNodeTypeOnly(const cNodeSCPI *node);
    static void addNodeAndChildrenToXml(const cNodeSCPI *scpiNode,
                                        QDomDocument &doc,
                                        QDomElement &parentXmlElement,
                                        const QStringList parentScpiNames);
    static QDomElement createXmlTag(QStringList scpiParentNameList,
                                    QDomDocument &doc,
                                    QString scpiName,
                                    const cNodeSCPI *scpiNode);
};

#endif // SCPIOLDNODESTATICFUNCTIONS_H
