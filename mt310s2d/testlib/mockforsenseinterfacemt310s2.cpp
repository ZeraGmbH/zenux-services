#include "mockforsenseinterfacemt310s2.h"

MockForSenseInterfaceMt310s2::MockForSenseInterfaceMt310s2() :
    MockPcbServer("mt310s2d")
{
    m_i2cSettings = std::make_unique<cI2CSettings>(getConfigReader());
    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_i2cSettings.get(), m_senseSettings.get()});

    m_systemInfo = std::make_unique<cSystemInfo>();

    m_senseInterface = std::make_unique<Mt310s2SenseInterface>(getSCPIInterface(), m_i2cSettings.get(), m_senseSettings.get(), m_systemInfo.get());
    m_clampInterface = std::make_unique<cClampInterface>(this, m_i2cSettings.get(), m_senseSettings.get(), m_senseInterface.get());
    setResources(ResourcesList{m_senseInterface.get()});
    start();
}
