#ifndef SCPIDELEGATE_H
#define SCPIDELEGATE_H

#include "scpiobject.h"
#include "protonetcommand.h"
#include "scpinotificationsubscriberhandler.h"
#include "notificationstring.h"
#include <scpi.h>
#include <QString>

class ScpiDelegate: public QObject, public ScpiObject
{
   Q_OBJECT
public:
   static std::shared_ptr<ScpiDelegate> create(const QString &cmdParent,
                                               const QString &cmd,
                                               quint8 type,
                                               std::shared_ptr<cSCPI> scpiInterface,
                                               quint16 cmdCode,
                                               NotificationString *notificationString = nullptr);
    ScpiDelegate(const QString &cmdParent,
                 const QString &cmd,
                 quint8 type,
                 quint16 cmdCode,
                 NotificationString *notificationString);
    ~ScpiDelegate();
    virtual bool executeSCPI(const QString&, QString&) override { return false; }
    virtual bool executeSCPI(ProtonetCommandPtr protoCmd);
    const QString &getCommand() const;
    NotificationString *getNotificationString();
    ScpiNotificationSubscriberHandler &getScpiNotificationSubscriberHandler();
public slots:
    void notifyAllSubscribers(QString newValue);
signals:
    void sigExecuteProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd);
    void sigNotifySubcriber(ScpiNotificationSubscriber subscriber, QString newValue);
private:
    quint16 m_nCmdCode;
    QString m_sCommand;
    ScpiNotificationSubscriberHandler m_notificationsHandler;
    NotificationString *m_notificationString = nullptr;
};

typedef std::shared_ptr<ScpiDelegate> ScpiDelegatePtr;

#endif // SCPIDELEGATE_H
