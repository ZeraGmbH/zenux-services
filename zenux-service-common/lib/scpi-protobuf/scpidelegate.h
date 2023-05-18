#ifndef SCPIDELEGATE_H
#define SCPIDELEGATE_H

#include "scpiobject.h"
#include "protonetcommand.h"
#include "scpinotificationsubscriberhandler.h"
#include "notificationstring.h"
#include <scpi.h>
#include <QString>

class cSCPIDelegate: public QObject, public cSCPIObject
{
   Q_OBJECT
public:
    cSCPIDelegate(QString cmdParent, QString cmd, quint8 type, cSCPI *scpiInterface, quint16 cmdCode, NotificationString *notificationString = nullptr);
    virtual bool executeSCPI(const QString&, QString&) override { return false; }
    virtual bool executeSCPI(cProtonetCommand* protoCmd);
    QString getCommand();
    NotificationString *getNotificationString();
    ScpiNotificationSubscriberHandler &getScpiNotificationSubscriberHandler();
public slots:
    void notifyAllSubscribers();
signals:
    void sigExecuteProtoScpi(int cmdCode, cProtonetCommand* protoCmd);
    void notify(ScpiNotificationSubscriber subscriber);
private:
    quint16 m_nCmdCode;
    QString m_sCommand;
    ScpiNotificationSubscriberHandler m_notificationsHandler;
    NotificationString *m_notificationString = nullptr;
};

#endif // SCPIDELEGATE_H
