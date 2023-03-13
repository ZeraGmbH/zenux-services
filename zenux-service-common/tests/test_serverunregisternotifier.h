#ifndef TEST_SERVERUNREGISTERNOTIFIER_H
#define TEST_SERVERUNREGISTERNOTIFIER_H

#include "mockatmel.h"
#include "pcbtestserver.h"
#include "adjustmentstatusnull.h"
#include "foutsettings.h"
#include "accumulatorinterface.h"
#include <memory>
#include <QObject>

class test_serverunregisternotifier : public QObject
{
    Q_OBJECT
public:
    test_serverunregisternotifier();
private slots:
    void init();

    void oneScpiConnection();
    void twoScpiConnections();

    void mtConfigLoaded();

    void scpiConnectionWithInternalScpiConnections();
private:
    cSCPIDelegate* getDelegate(QString cmd);

    cSCPI m_scpiInterface;
    std::unique_ptr<PCBTestServer> m_pcbServerTest;
    std::unique_ptr<MockAtmel> m_atmel;
    std::unique_ptr<AdjustmentStatusNull> m_adjustmentStatusNull;

    std::unique_ptr<Zera::XMLConfig::cReader> m_xmlConfigReader;
    std::unique_ptr<FOutSettings> m_foutSettings;
    std::unique_ptr<accumulatorSettings> m_accSettings;
};

#endif // TEST_SERVERUNREGISTERNOTIFIER_H
