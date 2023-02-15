#include "scpidelegate.h"

cSCPIDelegate::cSCPIDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode)
    :cSCPIObject(cmd, type), m_nCmdCode(cmdCode)
{
    m_sCommand = QString("%1:%2").arg(cmdParent, cmd);
    scpiInterface->insertScpiCmd(cmdParent.split(":"), this);
}

bool cSCPIDelegate::executeSCPI(cProtonetCommand *protoCmd)
{
    emit sigExecuteProtoScpi(m_nCmdCode, protoCmd);
    return true;
}

QString cSCPIDelegate::getCommand()
{
    return m_sCommand;
}

void cSCPIDelegate::addNotificationSubscriber(const ScpiNotificationSubscriber &subscriber)
{
    m_notificationsHandler.addSubscriber(subscriber);
}

void cSCPIDelegate::removeAllNotificationSubscribers(XiQNetPeer *netPeer)
{
    m_notificationsHandler.removeAllSubscribersFromAPeer(netPeer);
}
