#ifndef TESTPCBSERVERNOTIFICATIONS_H
#define TESTPCBSERVERNOTIFICATIONS_H

#include "pcbserver.h"
#include "abstractfactoryi2cctrl.h"

class TestPcbServerNotifications : public PCBServer
{
    Q_OBJECT
public:
    TestPcbServerNotifications(SettingsContainerPtr settings, cSCPI *scpiInterface,
                               AbstractFactoryI2cCtrlPtr ctrlFactory,
                               VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory);
    ~TestPcbServerNotifications();
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

#endif // TESTPCBSERVERNOTIFICATIONS_H
