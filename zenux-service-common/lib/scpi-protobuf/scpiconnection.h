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
protected:
    void removeSCPIConnections();
    void ensureTrailingColonOnNonEmptyParentNodes(QString &leadingNodes);
    void addDelegate(cSCPIDelegate *delegate);
    cSCPI* m_pSCPIInterface;
    QList<cSCPIDelegate*> m_DelegateList;
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd) = 0;
};

#endif // SCPICONNECTION_H
