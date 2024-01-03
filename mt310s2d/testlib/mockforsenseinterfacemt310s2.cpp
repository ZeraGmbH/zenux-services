#include "mockforsenseinterfacemt310s2.h"
#include "clampfactorytest.h"

MockForSenseInterfaceMt310s2::MockForSenseInterfaceMt310s2(AtmelPermissionTemplate *permissionQueryHandler) :
    MockPcbServer("mt310s2d")
{
    PermissionFunctions::setPermissionPinController(&m_permissionMock);
    enableEEPROMPermission();

    m_i2cSettings = std::make_unique<cI2CSettings>(getConfigReader());
    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_i2cSettings.get(), m_senseSettings.get()});

    m_systemController = std::make_shared<cATMELSysCtrl>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), 0);
    m_systemInfo = std::make_unique<Mt310s2SystemInfo>(m_systemController);

    m_senseInterface = std::make_unique<Mt310s2SenseInterface>(getSCPIInterface(),
                                                               m_i2cSettings.get(),
                                                               m_senseSettings.get(),
                                                               m_systemInfo.get(),
                                                               permissionQueryHandler);
    m_clampInterface = std::make_unique<cClampInterface>(this,
                                                         m_i2cSettings.get(),
                                                         m_senseSettings.get(),
                                                         m_senseInterface.get(),
                                                         permissionQueryHandler);
    setResources(ResourcesList{m_senseInterface.get()});

    m_systemInterface = std::make_unique<Mt310s2SystemInterface>(this,
                                                                 m_systemInfo.get(),
                                                                 m_senseSettings.get(),
                                                                 m_senseInterface.get(),
                                                                 m_systemController,
                                                                 nullptr,
                                                                 permissionQueryHandler);

    setScpiConnections(ScpiConnectionList{m_systemInterface.get(), m_clampInterface.get()});

    start();
}

void MockForSenseInterfaceMt310s2::enableEEPROMPermission()
{
    m_permissionMock.accessEnableAfter(0);
}

void MockForSenseInterfaceMt310s2::addClamp(int clampTypeNo, QString channelAlias1)
{
    ClampFactoryTest::setTestClampType(clampTypeNo);
    SenseSystem::cChannelSettings *channelSettingClamps = getSenseSettings()->findChannelSettingByAlias1(channelAlias1);
    getClampInterface()->addClamp(channelSettingClamps, I2cMultiplexerFactory::createNullMuxer());
}

void MockForSenseInterfaceMt310s2::removeAllClamps()
{
    // to execute as much production code as possible we use actualizeClampStatus
    getClampInterface()->actualizeClampStatus(0);
}
