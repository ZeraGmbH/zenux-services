#ifndef PCBTESTSERVER_H
#define PCBTESTSERVER_H

#include "pcbserver.h"
#include "atmelpermissionmock.h"

class PCBTestServer : public cPCBServer
{
    Q_OBJECT
public:
    PCBTestServer(ServerParams params, cSCPI *scpiInterface, AtmelPermissionMock *atmel);
    ~PCBTestServer();
    void insertScpiConnection(ScpiConnection *scpiConnection);
    void initTestSCPIConnections();
    void registerNotifier(QString inputCmd, int notifierId);
    void unregisterNotifier();
signals:
    void notificationSent(qint16 notifierId);
private slots:
    void doConfiguration() override;
    void onSendNotification(ScpiNotificationSubscriber subscriber) override;
};

#endif // PCBTESTSERVER_H
