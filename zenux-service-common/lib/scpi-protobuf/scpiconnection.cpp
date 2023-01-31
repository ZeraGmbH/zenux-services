#include "scpiconnection.h"

ScpiConnection::ScpiConnection(cSCPI *scpiInterface)
    : m_pSCPIInterface(scpiInterface)
{
}

ScpiConnection::~ScpiConnection()
{
    removeSCPIConnections();
}

void ScpiConnection::removeSCPIConnections()
{
    if(m_pSCPIInterface) {
        for (int i = 0; i < m_DelegateList.count(); i++) {
            cSCPIDelegate* ptr = m_DelegateList.at(i);
            m_pSCPIInterface->delSCPICmds(ptr->getCommand());
        }
    }
    for (int i = 0; i < m_DelegateList.count(); i++) {
        cSCPIDelegate* ptr = m_DelegateList.at(i);
        delete ptr;
    }
    m_DelegateList.clear();
}

void ScpiConnection::ensureTrailingColonOnNonEmptyParentNodes(QString &leadingNodes)
{
    if(!leadingNodes.isEmpty() && !leadingNodes.endsWith(":"))
        leadingNodes.append(":");
}

void ScpiConnection::addDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode)
{
    cSCPIDelegate *delegate = new cSCPIDelegate(cmdParent, cmd, type, scpiInterface, cmdCode);
    m_DelegateList.append(delegate);
    connect(delegate, &cSCPIDelegate::sigExecuteProtoScpi, this, &ScpiConnection::onExecuteProtoScpi);
}

void ScpiConnection::onExecuteProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    executeProtoScpi(cmdCode, protoCmd);
}

void ScpiConnection::onNotifierUnregistered(NotificationString *notifier)
{
}
