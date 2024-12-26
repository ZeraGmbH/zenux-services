#ifndef SCPIOLDNODESTATICFUNCTIONS_H
#define SCPIOLDNODESTATICFUNCTIONS_H

#include "scpi-zdsp.h"
#include <QStringList>
#include <QDomDocument>

class ScpiOldNodeStaticFunctions
{
public:
    static bool isNodeTypeOnly(const cNodeSCPI *node);
    static void addNodeAndChildrenToXml(const cNodeSCPI *node,
                                        QDomDocument &doc,
                                        QDomElement &rootElement,
                                        const QStringList parentNames);
    static QDomElement createCmdTag(QStringList childNames,
                                    QDomDocument &doc,
                                    QString childName,
                                    const cNodeSCPI *childNode);
};

#endif // SCPIOLDNODESTATICFUNCTIONS_H
