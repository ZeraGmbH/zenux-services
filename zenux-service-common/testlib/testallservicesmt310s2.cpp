#include "testallservicesmt310s2.h"
#include <timemachineobject.h>
#include "testfactorydevicenodedsp.h"
#include <tcpworkerfactory.h>

TestAllServicesMt310s2::TestAllServicesMt310s2(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    init(VeinTcp::TcpWorkerFactory::create(), ctrlFactory);
}

TestAllServicesMt310s2::TestAllServicesMt310s2(VeinTcp::AbstractTcpWorkerFactoryPtr tcpNetworkFactory,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    init(tcpNetworkFactory, ctrlFactory);
}

void TestAllServicesMt310s2::init(VeinTcp::AbstractTcpWorkerFactoryPtr tcpNetworkFactory,
                                  AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resman = new ResmanRunFacade(tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();;
    m_mt310s2d = new MockMt310s2d(ctrlFactory, tcpNetworkFactory);
    m_sec1000d = new MockSec1000d(tcpNetworkFactory);
    m_zdsp1d = new MockZdsp1d(std::make_shared<TestFactoryDeviceNodeDsp>(), tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();
}

TestAllServicesMt310s2::~TestAllServicesMt310s2()
{
    delete m_zdsp1d;
    delete m_sec1000d;
    delete m_mt310s2d;
    TimeMachineObject::feedEventLoop();
    delete m_resman;
    TimeMachineObject::feedEventLoop();
}
