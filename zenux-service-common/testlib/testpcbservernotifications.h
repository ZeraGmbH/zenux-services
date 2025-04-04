#ifndef TESTPCBSERVERNOTIFICATIONS_H
#define TESTPCBSERVERNOTIFICATIONS_H

#include "pcbserver.h"
#include "abstractfactoryi2cctrl.h"

class TestPcbServerNotifications : public PCBServer
{
    Q_OBJECT
public:
    TestPcbServerNotifications(SettingsContainerPtr settings,
                               AbstractFactoryI2cCtrlPtr ctrlFactory,
                               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    ~TestPcbServerNotifications();
    void insertScpiConnection(ScpiConnection *scpiConnection);
    void initTestSCPIConnections();
    void registerNotifier(QString inputCmd, int notifierId);
    void unregisterNotifier();
signals:
    void notificationSent(qint16 notifierId);
private slots:
    void onNotifySubscriber(ScpiNotificationSubscriber subscriber, QString newValue) override;
private:
    void doConfiguration();
};

#endif // TESTPCBSERVERNOTIFICATIONS_H
