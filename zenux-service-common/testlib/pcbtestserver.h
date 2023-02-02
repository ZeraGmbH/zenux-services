#ifndef PCBTESTSERVER_H
#define PCBTESTSERVER_H

#include "pcbserver.h"
#include "statusinterface.h"
#include "mockatmel.h"

class PCBTestServer : public cPCBServer
{
public:
    PCBTestServer(QString name, QString version, cSCPI *scpiInterface);
    ~PCBTestServer();
    void sendNotificationToClient(NotificationStructWithStringAndId notData) override;
private slots:
    void doConfiguration() override;
private:
    cStatusInterface *m_pStatusInterface;
    AdjustmentStatusInterface *m_adjustmentStatusNull;
    MockAtmel *atmel;
};

#endif // PCBTESTSERVER_H
