#ifndef PCBTESTSERVER_H
#define PCBTESTSERVER_H

#include "pcbserver.h"
#include "statusinterface.h"
#include "mockatmel.h"

class PCBTestServer : public cPCBServer
{
    Q_OBJECT
public:
    PCBTestServer(QString name, QString version, cSCPI *scpiInterface, MockAtmel *atmel);
    ~PCBTestServer();
signals:
    void notificationSent(qint16 notifierId);
private slots:
    void doConfiguration() override;
    void onSendNotification(ScpiNotificationSubscriber subscriber) override;
private:
    cStatusInterface *m_pStatusInterface;
    AdjustmentStatusInterface *m_adjustmentStatusNull;
};

#endif // PCBTESTSERVER_H
