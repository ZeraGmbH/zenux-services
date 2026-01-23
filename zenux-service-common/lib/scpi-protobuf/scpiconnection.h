#ifndef SCPICONNECTION_H
#define SCPICONNECTION_H

#include "scpidelegate.h"
#include "notificationstring.h"
#include "notificationvalue.h"
#include <QList>
#include <memory>

class ScpiConnection: public QObject // pure virtual base class for scpi model interfaces
{
    Q_OBJECT
public:
    ScpiConnection(std::shared_ptr<cSCPI> scpiInterface);
    virtual ~ScpiConnection();
    virtual void initSCPIConnection(QString leadingNodes) = 0;
signals:
    void valNotifier(NotificationValue* notifier);
    void cmdExecutionDone(ProtonetCommandPtr protoCmd);
    void sigNotifySubcriber(ScpiNotificationSubscriber subscriber, QString newValue);
    void removingSubscribers(VeinTcp::TcpPeer* peer, const QByteArray &clientID);
public slots:
    virtual void onNotifierRegistered(NotificationString* notifier);
    void onRemoveSubscribers(VeinTcp::TcpPeer *peer, const QByteArray &clientID);
protected:
    virtual void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) = 0;
    void removeSCPIConnections();
    void ensureTrailingColonOnNonEmptyParentNodes(QString &leadingNodes);
    void addDelegate(QString cmdParent, QString cmd, quint8 type, std::shared_ptr<cSCPI> scpiInterface, quint16 cmdCode, NotificationString *notificationString = nullptr);
    std::shared_ptr<cSCPI> m_scpiInterface;
    QList<ScpiDelegatePtr> m_DelegateList;
private slots:
    void onExecuteProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd);
};

#endif // SCPICONNECTION_H
