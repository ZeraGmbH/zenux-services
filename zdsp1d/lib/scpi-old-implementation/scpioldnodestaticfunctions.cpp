#include "scpioldnodestaticfunctions.h"
#include "scpi.h"
#include <scpinodestaticfunctions.h>

bool ScpiOldNodeStaticFunctions::isNodeTypeOnly(const cNodeSCPI *node)
{
    bool isCmd = node->m_nCmd != nixCmd;
    bool isQuery = node->m_nQuery != nixCmd;
    return !isCmd && !isQuery;
}

void ScpiOldNodeStaticFunctions::addNodeAndChildrenToXml(const cNodeSCPI *node,
                                                         QDomDocument &doc,
                                                         QDomElement &rootElement,
                                                         const QStringList parentNames)
{
    cNodeSCPI *childNode = node->getFirstChildNode();
    while(childNode) {
        QString childNameFull = childNode->getName();
        QStringList childNameListFull = parentNames + QStringList(childNameFull);
        QDomElement cmdTag = rootElement.firstChildElement(childNameFull);
        if(childNode->getType() != SCPI::isNode || cmdTag.isNull()) {
            cmdTag = ScpiOldNodeStaticFunctions::createCmdTag(childNameListFull,
                                                                          doc,
                                                                          childNameFull,
                                                                          childNode);
            QString typeInfo = ScpiNodeStaticFunctions::scpiTypeToString(childNode->getType());
            cmdTag.setAttribute("Type", typeInfo);
            rootElement.appendChild(cmdTag);
        }
        addNodeAndChildrenToXml(childNode, doc, cmdTag, childNameListFull);
        childNode = childNode->getNextNode();
    }
}

QDomElement ScpiOldNodeStaticFunctions::createCmdTag(QStringList childNames, QDomDocument &doc, QString childName, const cNodeSCPI *childNode)
{
    QDomElement cmdTag = doc.createElement(ScpiNodeStaticFunctions::makeValidXmlTag(childName));
    if(!isNodeTypeOnly(childNode))
        cmdTag.setAttribute("ScpiPath", childNames.join(":"));
    return cmdTag;
}
