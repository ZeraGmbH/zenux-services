#ifndef TEST_SERVERUNREGISTERNOTIFIER_H
#define TEST_SERVERUNREGISTERNOTIFIER_H

#include "testpcbservernotifications.h"
#include "testadjustmentstatusinterfacenull.h"
#include "foutsettings.h"
#include "accumulatorsettings.h"
#include <memory>

class test_serverunregisternotifier : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void oneScpiConnection();
    void twoScpiConnections();

    void mtConfigLoaded();

    void scpiConnectionWithInternalScpiConnections();
private:
    cSCPIDelegate* getDelegate(QString cmd);

    std::unique_ptr<TestPcbServerNotifications> m_pcbServerTest;
    std::unique_ptr<TestAdjustmentStatusInterfaceNull> m_adjustmentStatusNull;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;

    std::unique_ptr<Zera::XMLConfig::cReader> m_xmlConfigReader;
    std::unique_ptr<FOutSettings> m_foutSettings;
    std::unique_ptr<AccumulatorSettings> m_accSettings;
};

#endif // TEST_SERVERUNREGISTERNOTIFIER_H
