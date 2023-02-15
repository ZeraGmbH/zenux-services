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
    void removeAllScpiNotificationSubscribers(XiQNetPeer *netPeer);
signals:
    void strNotifier(NotificationString* notifier);
    void valNotifier(NotificationValue* notifier);
    void cmdExecutionDone(cProtonetCommand* protoCmd);
public slots:
    virtual void onNotifierRegistered(NotificationString* notifier);
    virtual void onNotifierUnregistered(NotificationString* notifier);
protected:
    virtual void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) = 0;
    void removeSCPIConnections();
    void ensureTrailingColonOnNonEmptyParentNodes(QString &leadingNodes);
    void addDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode);
    cSCPI* m_pSCPIInterface;
    QList<cSCPIDelegate*> m_DelegateList;
private slots:
    void onExecuteProtoScpi(int cmdCode, cProtonetCommand* protoCmd);
};

#endif // SCPICONNECTION_H
