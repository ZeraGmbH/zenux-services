#include "testserverforsenseinterfacemt310s2.h"
#include "clampfactorytest.h"
#include "mt310s2senseinterface.h"
#include "mt310s2systeminfomock.h"
#include <i2cmultiplexerfactory.h>

TestServerForSenseInterfaceMt310s2::TestServerForSenseInterfaceMt310s2(AbstractFactoryI2cCtrlPtr ctrlFactory, bool systemInfoMock) :
    TestPcbServer("mt310s2d")
{
    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_senseSettings.get()});
    
    if(systemInfoMock)
        m_systemInfo = std::make_unique<Mt310s2SystemInfoMock>(ctrlFactory);
    else
        m_systemInfo = std::make_unique<Mt310s2SystemInfo>(ctrlFactory);

    m_senseInterface = std::make_unique<Mt310s2SenseInterface>(getSCPIInterface(),
                                                               m_settings->getI2cSettings(),
                                                               m_senseSettings.get(),
                                                               m_systemInfo.get(),
                                                               ctrlFactory);
    m_clampInterface = std::make_unique<cClampInterface>(this,
                                                         m_settings->getI2cSettings(),
                                                         m_senseSettings.get(),
                                                         m_senseInterface.get(),
                                                         ctrlFactory);
    setResources(ResourcesList{m_senseInterface.get()});

    m_systemInterface = std::make_unique<Mt310s2SystemInterface>(this,
                                                                 m_systemInfo.get(),
                                                                 m_senseSettings.get(),
                                                                 m_senseInterface.get(),
                                                                 ctrlFactory,
                                                                 nullptr);

    setScpiConnections(ScpiConnectionList{m_systemInterface.get(), m_clampInterface.get()});

    start();
}

cClamp * TestServerForSenseInterfaceMt310s2::addClamp(int clampTypeNo, QString channelAlias1)
{
    ClampFactoryTest::setTestClampType(clampTypeNo);
    SenseSystem::cChannelSettings *channelSettingClamps = getSenseSettings()->findChannelSettingByAlias1(channelAlias1);
    return getClampInterface()->addClamp(channelSettingClamps, I2cMultiplexerFactory::createNullMuxer());
}

void TestServerForSenseInterfaceMt310s2::removeAllClamps()
{
    // to execute as much production code as possible we use actualizeClampStatus
    getClampInterface()->actualizeClampStatus(0);
}
