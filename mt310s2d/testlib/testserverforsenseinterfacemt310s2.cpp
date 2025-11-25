#include "testserverforsenseinterfacemt310s2.h"
#include "mockclampplugger.h"
#include "mockeepromi2cfactory.h"
#include "mockeepromdevice.h"
#include "mt310s2senseinterface.h"
#include "mt310s2systeminfomock.h"
#include <i2cmultiplexerfactory.h>

TestServerForSenseInterfaceMt310s2::TestServerForSenseInterfaceMt310s2(AbstractFactoryI2cCtrlPtr ctrlFactory,
                                                                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                                                       const QString &serviceNameForAlternateDevice,
                                                                       bool systemInfoMock) :
    TestPcbServer(serviceNameForAlternateDevice, tcpNetworkFactory)
{
    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_senseSettings.get()});
    
    if(systemInfoMock)
        m_systemInfo = std::make_unique<Mt310s2SystemInfoMock>(ctrlFactory);
    else
        m_systemInfo = std::make_unique<Mt310s2SystemInfo>(ctrlFactory);

    AbstractEepromI2cFactoryPtr adjMemFactory = std::make_shared<MockEepromI2cFactory>();
    const I2cSettings *i2cSettings = m_settings->getI2cSettings();
    EepromI2cDeviceInterfacePtr adjEeprom = adjMemFactory->createEeprom(
        {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
        i2cSettings->getEepromByteSize());

    m_senseInterface = std::make_unique<Mt310s2SenseInterface>(m_scpiInterface,
                                                               m_senseSettings.get(),
                                                               std::move(adjEeprom),
                                                               m_systemInfo.get(),
                                                               SettingsContainer::createChannelRangeFactory(serviceNameForAlternateDevice),
                                                               ctrlFactory);
    m_clampInterface = std::make_unique<cClampInterface>(this,
                                                         m_senseSettings.get(),
                                                         m_senseInterface.get(),
                                                         i2cSettings,
                                                         adjMemFactory,
                                                         ctrlFactory);
    m_samplingInterface = std::make_unique<cSamplingInterface>(m_scpiInterface,
                                                               m_settings->getSamplingSettings(),
                                                               ctrlFactory);
    setResources(ResourcesList{m_senseInterface.get(), m_samplingInterface.get()});

    m_systemInterface = std::make_unique<Mt310s2SystemInterface>(this,
                                                                 m_systemInfo.get(),
                                                                 m_senseSettings.get(),
                                                                 m_senseInterface.get(),
                                                                 ctrlFactory,
                                                                 nullptr);

    setScpiConnections(ScpiConnectionList{m_systemInterface.get(), m_clampInterface.get()});

    start();
}

cClamp *TestServerForSenseInterfaceMt310s2::addClamp(int clampType, QString channelAlias)
{
    MockClampPlugger clampPlugger(getSenseSettings(), m_settings->getI2cSettings());
    clampPlugger.initClampAdjMem(clampType, channelAlias);

    SenseSystem::cChannelSettings *channelSettingClamps = getSenseSettings()->findChannelSettingByAlias1(channelAlias);
    return getClampInterface()->tryAddClamp(channelSettingClamps);
}

void TestServerForSenseInterfaceMt310s2::removeAllClamps()
{
    // to execute as much production code as possible we use actualizeClampStatus
    getClampInterface()->actualizeClampStatus(0);
}
