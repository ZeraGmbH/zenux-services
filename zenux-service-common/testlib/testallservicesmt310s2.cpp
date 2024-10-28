#include "testallservicesmt310s2.h"
#include <timemachineobject.h>
#include "testfactorydevicenodedsp.h"
#include <tcpworkerfactory.h>

TestAllServicesMt310s2::TestAllServicesMt310s2(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    init(VeinTcp::TcpWorkerFactory::create(), ctrlFactory);
}

TestAllServicesMt310s2::TestAllServicesMt310s2(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    init(tcpWorkerFactory, ctrlFactory);
}

void TestAllServicesMt310s2::init(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory,
                                  AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resman = new ResmanRunFacade(tcpWorkerFactory);
    TimeMachineObject::feedEventLoop();;
    m_mt310s2d = new MockMt310s2d(ctrlFactory, tcpWorkerFactory);
    m_sec1000d = new MockSec1000d(tcpWorkerFactory);
    m_zdsp1d = new MockZdsp1d(std::make_shared<TestFactoryDeviceNodeDsp>(), tcpWorkerFactory);
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
