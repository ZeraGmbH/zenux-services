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

cSEC1000dServer *TestAllServicesMt310s2::getSecServer()
{
    return m_sec1000d->getServer();
}

void TestAllServicesMt310s2::fireHotplugInterrupt(const ChannelAliasHotplugDeviceNameMap &deviceMap)
{
    m_mt310s2d->fireHotplugInterrupt(deviceMap);
}

cSenseSettingsPtr TestAllServicesMt310s2::getSenseSettings()
{
    return m_mt310s2d->getSenseSettings();
}

void TestAllServicesMt310s2::addStandardEmobControllers(const QStringList &channelAliases)
{
    m_mt310s2d->addStandardEmobControllers(channelAliases);
}

void TestAllServicesMt310s2::removeAllHotplugDevices()
{
    m_mt310s2d->removeAllHotplugDevices();
}

void TestAllServicesMt310s2::addClamps(const QList<clampParam> &clampParams)
{
    m_mt310s2d->addClamps(clampParams);
}

AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap TestAllServicesMt310s2::getCurrentHotplugMap() const
{
    return m_mt310s2d->getCurrentHotplugMap();
}
