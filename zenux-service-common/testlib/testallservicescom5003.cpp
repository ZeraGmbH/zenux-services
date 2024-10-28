#include "testallservicescom5003.h"
#include <timemachineobject.h>
#include "testfactorydevicenodedsp.h"
#include <tcpworkerfactory.h>

TestAllServicesCom5003::TestAllServicesCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    init(VeinTcp::TcpWorkerFactory::create(), ctrlFactory);
}

TestAllServicesCom5003::TestAllServicesCom5003(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    init(tcpWorkerFactory, ctrlFactory);
}

void TestAllServicesCom5003::init(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory, AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resman = new ResmanRunFacade(tcpWorkerFactory);
    TimeMachineObject::feedEventLoop();
    m_mockcom5003d = new MockCom5003d(ctrlFactory, tcpWorkerFactory);
    m_sec1000d = new MockSec1000d(tcpWorkerFactory);
    m_zdsp1d = new MockZdsp1d(std::make_shared<TestFactoryDeviceNodeDsp>(), tcpWorkerFactory);
    TimeMachineObject::feedEventLoop();
}

TestAllServicesCom5003::~TestAllServicesCom5003()
{
    delete m_zdsp1d;
    delete m_sec1000d;
    delete m_mockcom5003d;
    TimeMachineObject::feedEventLoop();
    delete m_resman;
    TimeMachineObject::feedEventLoop();
}
