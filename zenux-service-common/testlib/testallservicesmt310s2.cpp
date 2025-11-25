#include "testallservicesmt310s2.h"
#include <timemachineobject.h>
#include "testfactorydevicenodedsp.h"
#include <tcpnetworkfactory.h>

TestAllServicesMt310s2::TestAllServicesMt310s2(const QString &serviceNameForAlternateDevice, TestFactoryI2cCtrlPtr testCtrlFactory)
{
    init(serviceNameForAlternateDevice, VeinTcp::TcpNetworkFactory::create(), testCtrlFactory);
}

TestAllServicesMt310s2::TestAllServicesMt310s2(const QString &serviceNameForAlternateDevice,
                                               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                               TestFactoryI2cCtrlPtr testCtrlFactory)
{
    init(serviceNameForAlternateDevice, tcpNetworkFactory, testCtrlFactory);
}

void TestAllServicesMt310s2::init(const QString &serviceNameForAlternateDevice,
                                  VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                  TestFactoryI2cCtrlPtr testCtrlFactory)
{
    m_resman = new ResmanRunFacade(tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();
    m_mt310s2d = new MockMt310s2d(testCtrlFactory, tcpNetworkFactory, serviceNameForAlternateDevice);
    m_sec1000d = new MockSec1000d(tcpNetworkFactory, cSEC1000dServer::Mtxxxs2EcUnitCount);
    m_zdsp1d = new MockZdsp1d(std::make_shared<TestFactoryDeviceNodeDsp>(), tcpNetworkFactory);
    TimeMachineObject::feedEventLoop();
    m_testCtrlFactory = testCtrlFactory;
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

void TestAllServicesMt310s2::setRangeGetSetDelay(int rangeGetSetDelay)
{
    m_testCtrlFactory->setRangeGetSetDelay(rangeGetSetDelay);
}

ZDspServer *TestAllServicesMt310s2::getZdsp1dServer()
{
    return m_zdsp1d->getServer();
}

void TestAllServicesMt310s2::fireHotplugInterruptControllerOnly(const QStringList &channelAliases)
{
    m_mt310s2d->fireHotplugInterruptControllerOnly(channelAliases);
}

void TestAllServicesMt310s2::fireHotplugInterrupt(const ChannelAliasHotplugDeviceNameMap &infoMap)
{
    m_mt310s2d->fireHotplugInterrupt(infoMap);
}
