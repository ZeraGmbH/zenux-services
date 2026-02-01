#include "scpidelegate.h"

std::shared_ptr<ScpiDelegate> ScpiDelegate::create(const QString &cmdParent,
                                                   const QString &cmd,
                                                   quint8 type,
                                                   std::shared_ptr<cSCPI> scpiInterface,
                                                   quint16 cmdCode,
                                                   NotificationString *notificationString)
{
    ScpiDelegatePtr delegate = std::make_shared<ScpiDelegate>(
        cmdParent,
        cmd,
        type,
        cmdCode,
        notificationString);
    scpiInterface->insertScpiCmd(cmdParent.split(":",Qt::SkipEmptyParts), delegate);
    return delegate;
}

ScpiDelegate::ScpiDelegate(const QString &cmdParent,
                           const QString &cmd,
                           quint8 type,
                           quint16 cmdCode,
                           NotificationString *notificationString) :
    ScpiObject(cmd, type),
    m_nCmdCode(cmdCode),
    m_sCommand(cmdParent.isEmpty() ? cmd : QString("%1:%2").arg(cmdParent, cmd)),
    m_notificationString(notificationString)
{
    if (m_notificationString)
        connect(m_notificationString, &NotificationString::valueChanged, this, &ScpiDelegate::notifyAllSubscribers);
}

ScpiDelegate::~ScpiDelegate()
{

}

bool ScpiDelegate::executeSCPI(ProtonetCommandPtr protoCmd)
{
    emit sigExecuteProtoScpi(m_nCmdCode, protoCmd);
    return true;
}

QString ScpiDelegate::getCommand()
{
    return m_sCommand;
}

NotificationString *ScpiDelegate::getNotificationString()
{
    return m_notificationString;
}

ScpiNotificationSubscriberHandler &ScpiDelegate::getScpiNotificationSubscriberHandler()
{
    return m_notificationsHandler;
}

void ScpiDelegate::notifyAllSubscribers(QString newValue)
{
    for(int i = 0; i < m_notificationsHandler.getTotalSubscribers(); i++)
        emit sigNotifySubcriber(m_notificationsHandler.getSubscriber(i), newValue);
}
