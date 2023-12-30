#include "mockforsenseinterfacemt310s2.h"

MockForSenseInterfaceMt310s2::MockForSenseInterfaceMt310s2() :
    MockPcbServer("mt310s2d")
{
    m_i2cSettings = std::make_unique<cI2CSettings>(getConfigReader());
    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_i2cSettings.get(), m_senseSettings.get()});

    m_systemController = std::make_shared<cATMELSysCtrl>(m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::sysCtrlI2cAddress), 0);
    m_systemInfo = std::make_unique<Mt310s2SystemInfo>(m_systemController);

    m_senseInterface = std::make_unique<Mt310s2SenseInterface>(getSCPIInterface(), m_i2cSettings.get(), m_senseSettings.get(), m_systemInfo.get());
    m_clampInterface = std::make_unique<cClampInterface>(this, m_i2cSettings.get(), m_senseSettings.get(), m_senseInterface.get());
    setResources(ResourcesList{m_senseInterface.get()});

    m_systemInterface = std::make_unique<Mt310s2SystemInterface>(this, m_systemInfo.get(), m_senseSettings.get(), m_senseInterface.get(), nullptr, nullptr);

    setScpiConnections(ScpiConnectionList{m_systemInterface.get(), m_clampInterface.get()});
    start();
}
