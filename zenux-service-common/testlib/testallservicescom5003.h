#ifndef TESTALLSERVICESCOM5003_H
#define TESTALLSERVICESCOM5003_H

#include "abstracttestallservices.h"
#include "resmanrunfacade.h"
#include "mockcom5003d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"
#include "testfactoryi2cctrl.h"

class TestAllServicesCom5003 : public AbstractTestAllServices
{
public:
    TestAllServicesCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true));
    TestAllServicesCom5003(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<TestFactoryI2cCtrl>(true));
    virtual ~TestAllServicesCom5003();
private:
    void init(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory, AbstractFactoryI2cCtrlPtr ctrlFactory);
    ResmanRunFacade *m_resman;
    MockCom5003d *m_mockcom5003d;
    MockSec1000d *m_sec1000d;
    MockZdsp1d *m_zdsp1d;
};

#endif // TESTALLSERVICESCOM5003_H
