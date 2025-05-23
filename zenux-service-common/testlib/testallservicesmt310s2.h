#ifndef TESTALLSERVICESMT310S2_H
#define TESTALLSERVICESMT310S2_H

#include "abstracttestallservices.h"
#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"
#include "testfactoryi2cctrl.h"

class TestAllServicesMt310s2 : public AbstractTestAllServices
{
public:
    TestAllServicesMt310s2(const QString &serviceNameForAlternateDevice,
                           AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true));
    TestAllServicesMt310s2(const QString &serviceNameForAlternateDevice,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true));
    virtual ~TestAllServicesMt310s2();

    ZDspServer *getZdsp1dServer() override;
private:
    void init(const QString &serviceNameForAlternateDevice,
              VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
              AbstractFactoryI2cCtrlPtr ctrlFactory);
    ResmanRunFacade *m_resman;
    MockMt310s2d *m_mt310s2d;
    MockSec1000d *m_sec1000d;
    MockZdsp1d *m_zdsp1d;
};

#endif // TESTALLSERVICESMT310S2_H
