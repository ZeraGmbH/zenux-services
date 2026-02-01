#include "scpiconnection.h"

ScpiConnection::ScpiConnection(std::shared_ptr<cSCPI> scpiInterface)
    : m_scpiInterface(scpiInterface)
{
}

ScpiConnection::~ScpiConnection()
{
    removeSCPIConnections();
}

void ScpiConnection::removeSCPIConnections()
{
    if(m_scpiInterface) {
        for (int i = 0; i < m_DelegateList.count(); i++) {
            ScpiDelegatePtr ptr = m_DelegateList.at(i);
            m_scpiInterface->delSCPICmds(ptr->getCommand());
        }
    }
    m_DelegateList.clear();
}

QString ScpiConnection::appendTrailingColonOnNonEmptyParentNodes(const QString &leadingNodes)
{
    QString adjLeadNodes = leadingNodes;
    if(!adjLeadNodes.isEmpty() && !adjLeadNodes.endsWith(":"))
        adjLeadNodes.append(":");
    return adjLeadNodes;
}

void ScpiConnection::addDelegate(const QString &cmdParent,
                                 const QString &cmd,
                                 quint8 type,
                                 std::shared_ptr<cSCPI> scpiInterface,
                                 quint16 cmdCode,
                                 NotificationString *notificationString)
{
    ScpiDelegatePtr delegate = ScpiDelegate::create(cmdParent, cmd, type, scpiInterface, cmdCode, notificationString);
    m_DelegateList.append(delegate);
    connect(delegate.get(), &ScpiDelegate::sigExecuteProtoScpi, this, &ScpiConnection::onExecuteProtoScpi);
    connect(delegate.get(), &ScpiDelegate::sigNotifySubcriber, this, &ScpiConnection::sigNotifySubcriber);
}

void ScpiConnection::onExecuteProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    executeProtoScpi(cmdCode, protoCmd);
}

void ScpiConnection::onNotifierRegistered(NotificationString *notifier)
{
}

void ScpiConnection::onRemoveSubscribers(VeinTcp::TcpPeer *peer, const QByteArray &clientID)
{
    for (int i = 0; i < m_DelegateList.count(); i++) {
        m_DelegateList.at(i)->getScpiNotificationSubscriberHandler().removeAllSubscribers(peer, clientID);
    }
    emit removingSubscribers(peer, clientID);
}
