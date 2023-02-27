#ifndef SCPICONNECTION_H
#define SCPICONNECTION_H

#include "scpidelegate.h"
#include "notificationstring.h"
#include "notificationvalue.h"
#include <QList>

class ScpiConnection: public QObject // pure virtual base class for scpi model interfaces
{
    Q_OBJECT
public:
    ScpiConnection(cSCPI* scpiInterface);
    virtual ~ScpiConnection();
    virtual void initSCPIConnection(QString leadingNodes) = 0;
signals:
    void strNotifier(NotificationString* notifier);
    void valNotifier(NotificationValue* notifier);
    void cmdExecutionDone(cProtonetCommand* protoCmd);
    void sendNotification(ScpiNotificationSubscriber subscriber);
    void removingSubscribers(XiQNetPeer* peer, const QByteArray &clientID);
public slots:
    virtual void onNotifierRegistered(NotificationString* notifier);
    void onRemoveSubscribers(XiQNetPeer* peer, const QByteArray &clientID);
protected:
    virtual void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) = 0;
    void removeSCPIConnections();
    void ensureTrailingColonOnNonEmptyParentNodes(QString &leadingNodes);
    void addDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode, NotificationString *notificationString = nullptr);
    cSCPI* m_pSCPIInterface;
    QList<cSCPIDelegate*> m_DelegateList;
private slots:
    void onExecuteProtoScpi(int cmdCode, cProtonetCommand* protoCmd);
};

#endif // SCPICONNECTION_H
