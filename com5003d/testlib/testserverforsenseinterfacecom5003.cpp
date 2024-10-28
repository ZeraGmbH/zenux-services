#include "testserverforsenseinterfacecom5003.h"
#include "com5003senseinterface.h"
#include "testsysteminfo.h"

TestServerForSenseInterfaceCom5003::TestServerForSenseInterfaceCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory,
                                                                       VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory,
                                                                       bool systemInfoMock) :
    TestPcbServer("com5003d", tcpWorkerFactory)
{
    if(systemInfoMock)
        m_systemInfo = std::make_unique<TestSystemInfo>(ctrlFactory);
    else
        m_systemInfo = std::make_unique<SystemInfo>(ctrlFactory);

    m_senseSettings = std::make_unique<cSenseSettings>(getConfigReader(), 8);
    setXmlSettings(XmlSettingsList{m_senseSettings.get()});

    m_senseInterface = std::make_unique<Com5003SenseInterface>(getSCPIInterface(),
                                                               m_settings->getI2cSettings(),
                                                               getRmConnection(),
                                                               m_settings->getEthSettings(),
                                                               m_senseSettings.get(),
                                                               m_systemInfo.get(),
                                                               ctrlFactory);
    setResources(ResourcesList{m_senseInterface.get()});

    // no resources / scpi interfaces from here
    m_systemInterface = std::make_unique<Com5003SystemInterface>(this, m_systemInfo.get(), m_senseInterface.get(), ctrlFactory);
    setScpiConnections(ScpiConnectionList{m_systemInterface.get()});

    start();
}
