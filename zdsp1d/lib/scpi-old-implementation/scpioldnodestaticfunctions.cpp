#include "scpioldnodestaticfunctions.h"
#include <scpinodestaticfunctions.h>

bool ScpiOldNodeStaticFunctions::isNodeTypeOnly(const cNodeSCPI *node)
{
    bool isCmd = node->m_nCmd != nixCmd;
    bool isQuery = node->m_nQuery != nixCmd;
    return !isCmd && !isQuery;
}

void ScpiOldNodeStaticFunctions::addNodeAndChildrenToXml(const cNodeSCPI *scpiNode,
                                                         QDomDocument &doc,
                                                         QDomElement &parentXmlElement,
                                                         const QStringList parentScpiNames)
{
    // Set XML tag for this node
    QString scpiName = scpiNode->getName();
    QDomElement currXmlElem = parentXmlElement.firstChildElement(scpiName);
    if(currXmlElem.isNull()) { // do not re-create those created by new implementation
        currXmlElem = ScpiOldNodeStaticFunctions::createXmlTag(parentScpiNames,
                                                               doc,
                                                               scpiName,
                                                               scpiNode);
        parentXmlElement.appendChild(currXmlElem);
    }
    // Iterate children
    cNodeSCPI *childScpiNode = scpiNode->getFirstChildNode();
    if(childScpiNode)
        addNodeAndChildrenToXml(childScpiNode,
                                doc,
                                currXmlElem,
                                parentScpiNames + QStringList(scpiName));
    // Iterate neighbours
    cNodeSCPI *nextScpiNode = scpiNode->getNextNode();
    while(nextScpiNode) {
        addNodeAndChildrenToXml(nextScpiNode,
                                doc,
                                parentXmlElement,
                                parentScpiNames);
        nextScpiNode = nextScpiNode->getNextNode();
    }
}

QDomElement ScpiOldNodeStaticFunctions::createXmlTag(QStringList scpiParentNameList,
                                                     QDomDocument &doc,
                                                     QString scpiName,
                                                     const cNodeSCPI *scpiNode)
{
    QDomElement cmdTag = doc.createElement(ScpiNodeStaticFunctions::makeValidXmlTag(scpiName));
    if(!isNodeTypeOnly(scpiNode))
        cmdTag.setAttribute("ScpiPath", (scpiParentNameList + QStringList(scpiName)).join(":"));
    QString typeInfo = ScpiNodeStaticFunctions::scpiTypeToString(scpiNode->getType());
    cmdTag.setAttribute("Type", typeInfo);
    return cmdTag;
}
