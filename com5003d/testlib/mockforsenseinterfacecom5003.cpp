#include "mockforsenseinterfacecom5003.h"

MockForSenseInterfaceCom5003::MockForSenseInterfaceCom5003() :
    MockPcbServer("com5003d")
{
    m_i2cSettings = std::make_unique<cI2CSettings>(getConfigReader());

    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_i2cSettings.get(), m_senseSettings.get()});

    m_senseInterface = std::make_unique<Com5003SenseInterface>(getSCPIInterface(), getRmConnection(), &m_ethSettings, m_senseSettings.get());
    setResources(ResourcesList{m_senseInterface.get()});

    // no resources / scpi interfaces from here
    m_systemInfo = std::make_unique<cSystemInfo>();
    // On MT this is handled directly by sense interface.
    // Here we have to inject sense interface into Com5003Adjustment
    m_adjustment = std::make_unique<Com5003Adjustment>(m_systemInfo.get(), m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress));
    m_adjustment->addAdjXMLObject(m_senseInterface.get());

    start();
}
