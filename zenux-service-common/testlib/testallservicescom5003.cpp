#include "testallservicescom5003.h"
#include <timemachineobject.h>
#include "testfactorydevicenodedsp.h"
#include <tcpnetworkfactory.h>

TestAllServicesCom5003::TestAllServicesCom5003(TestFactoryI2cCtrlPtr testCtrlFactory)
{
    init(VeinTcp::TcpNetworkFactory::create(), testCtrlFactory);
}

TestAllServicesCom5003::TestAllServicesCom5003(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                               TestFactoryI2cCtrlPtr testCtrlFactory)
{
    init(tcpNetworkFactory, testCtrlFactory);
}

void TestAllServicesCom5003::init(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory, TestFactoryI2cCtrlPtr testCtrlFactory)
{
    m_resman = new ResmanRunFacade(tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();
    m_mockcom5003d = new MockCom5003d(testCtrlFactory, tcpNetworkFactory);
    m_sec1000d = new MockSec1000d(tcpNetworkFactory, cSEC1000dServer::Com5003EcUnitCount);
    m_zdsp1d = new MockZdsp1d(std::make_shared<TestFactoryDeviceNodeDsp>(), tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();
    m_testCtrlFactory = testCtrlFactory;
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

void TestAllServicesCom5003::setRangeGetSetDelay(int rangeGetSetDelay)
{
    m_testCtrlFactory->setRangeGetSetDelay(rangeGetSetDelay);
}

ZDspServer *TestAllServicesCom5003::getZdsp1dServer()
{
    return m_zdsp1d->getServer();
}

void TestAllServicesCom5003::fireHotplugInterrupt(const QStringList &channelAliases)
{
    Q_UNUSED(channelAliases)
    qWarning("Hotplug not supported on COM5003 yet!");
}

void TestAllServicesCom5003::fireHotplugInterruptControllerName(const ChannelAliasHotplugDeviceNameMap &infoMap)
{
    Q_UNUSED(infoMap)
    qWarning("Hotplug not supported on COM5003 yet!");
}
