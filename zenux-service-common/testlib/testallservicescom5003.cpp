#include "testallservicescom5003.h"
#include <timemachineobject.h>
#include "testfactorydevicenodedsp.h"
#include <tcpnetworkfactory.h>

TestAllServicesCom5003::TestAllServicesCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    init(VeinTcp::TcpNetworkFactory::create(), ctrlFactory);
}

TestAllServicesCom5003::TestAllServicesCom5003(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                               AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    init(tcpNetworkFactory, ctrlFactory);
}

void TestAllServicesCom5003::init(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory, AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resman = new ResmanRunFacade(tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();
    m_mockcom5003d = new MockCom5003d(ctrlFactory, tcpNetworkFactory);
    m_sec1000d = new MockSec1000d(tcpNetworkFactory, cSEC1000dServer::Com5003EcUnitCount);
    m_zdsp1d = new MockZdsp1d(std::make_shared<TestFactoryDeviceNodeDsp>(), tcpNetworkFactory);
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

ZDspServer *TestAllServicesCom5003::getZdsp1dServer()
{
    return m_zdsp1d->getServer();
}
