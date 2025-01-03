#include "scpidelegate.h"

cSCPIDelegate::cSCPIDelegate(QString cmdParent,
                             QString cmd,
                             quint8 type,
                             std::shared_ptr<cSCPI> scpiInterface,
                             quint16 cmdCode,
                             NotificationString *notificationString) :
    cSCPIObject(cmd, type),
    m_nCmdCode(cmdCode),
    m_notificationString(notificationString)
{
    scpiInterface->insertScpiCmd(cmdParent.split(":",Qt::SkipEmptyParts), this);
    if (m_notificationString)
        connect(m_notificationString, &NotificationString::valueChanged, this, &cSCPIDelegate::notifyAllSubscribers);
}

bool cSCPIDelegate::executeSCPI(cProtonetCommand *protoCmd)
{
    emit sigExecuteProtoScpi(m_nCmdCode, protoCmd);
    return true;
}

NotificationString *cSCPIDelegate::getNotificationString()
{
    return m_notificationString;
}

ScpiNotificationSubscriberHandler &cSCPIDelegate::getScpiNotificationSubscriberHandler()
{
    return m_notificationsHandler;
}

void cSCPIDelegate::notifyAllSubscribers(QString newValue)
{
    for(int i = 0; i < m_notificationsHandler.getTotalSubscribers(); i++)
        emit sigNotifySubcriber(m_notificationsHandler.getSubscriber(i), newValue);
}
