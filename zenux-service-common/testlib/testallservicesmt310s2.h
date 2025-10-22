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
                           TestFactoryI2cCtrlPtr testCtrlFactory = std::make_shared<TestFactoryI2cCtrl>(true));
    TestAllServicesMt310s2(const QString &serviceNameForAlternateDevice,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           TestFactoryI2cCtrlPtr testCtrlFactory = std::make_shared<TestFactoryI2cCtrl>(true));
    virtual ~TestAllServicesMt310s2();

    void setRangeGetSetDelay(int rangeGetSetDelay) override;
    ZDspServer *getZdsp1dServer() override;
private:
    void init(const QString &serviceNameForAlternateDevice,
              VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
              TestFactoryI2cCtrlPtr ctrlFactory);
    ResmanRunFacade *m_resman;
    MockMt310s2d *m_mt310s2d;
    MockSec1000d *m_sec1000d;
    MockZdsp1d *m_zdsp1d;
    TestFactoryI2cCtrlPtr m_testCtrlFactory;
};

#endif // TESTALLSERVICESMT310S2_H
