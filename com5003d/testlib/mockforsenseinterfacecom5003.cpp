#include "mockforsenseinterfacecom5003.h"

MockForSenseInterfaceCom5003::MockForSenseInterfaceCom5003(AtmelCtrlFactoryInterfacePrt ctrlFactory) :
    MockPcbServer("com5003d")
{
    m_i2cSettings = std::make_unique<cI2CSettings>(getConfigReader());
    m_systemInfo = std::make_unique<SystemInfo>(ctrlFactory);

    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_i2cSettings.get(), m_senseSettings.get()});

    m_senseInterface = std::make_unique<Com5003SenseInterface>(getSCPIInterface(),
                                                               m_i2cSettings.get(),
                                                               getRmConnection(),
                                                               &m_ethSettings,
                                                               m_senseSettings.get(),
                                                               m_systemInfo.get(),
                                                               ctrlFactory);
    setResources(ResourcesList{m_senseInterface.get()});

    // no resources / scpi interfaces from here
    m_systemInterface = std::make_unique<Com5003SystemInterface>(this, m_systemInfo.get(), m_senseInterface.get(), ctrlFactory);
    setScpiConnections(ScpiConnectionList{m_systemInterface.get()});

    start();
}
