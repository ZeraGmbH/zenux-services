#include "testserverforsenseinterfacecom5003.h"
#include "com5003senseinterface.h"
#include "mockeepromi2cfactory.h"
#include "testsysteminfo.h"

TestServerForSenseInterfaceCom5003::TestServerForSenseInterfaceCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory,
                                                                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                                                       const QString &serviceNameForAlternateDevice,
                                                                       bool systemInfoMock) :
    TestPcbServer(serviceNameForAlternateDevice, tcpNetworkFactory)
{
    PermissionFunctions::setPermissionCtrlFactory(ctrlFactory);
    if(systemInfoMock)
        m_systemInfo = std::make_unique<TestSystemInfo>(ctrlFactory);
    else
        m_systemInfo = std::make_unique<SystemInfo>(ctrlFactory);

    AbstractEepromI2cFactoryPtr adjMemFactory = std::make_shared<MockEepromI2cFactory>();
    I2cSettingsPtr i2cSettings = m_settings->getI2cSettings();
    EepromI2cDeviceInterfacePtr adjEeprom = adjMemFactory->createEeprom(
        {i2cSettings->getDeviceNode(), i2cSettings->getI2CAdress(i2cSettings::flashlI2cAddress)},
        i2cSettings->getEepromByteSize());
    m_senseInterface = std::make_unique<Com5003SenseInterface>(m_scpiInterface,
                                                               m_settings->getSenseSettings(),
                                                               std::move(adjEeprom),
                                                               m_systemInfo.get(),
                                                               SettingsContainer::createChannelRangeFactory(serviceNameForAlternateDevice),
                                                               ctrlFactory);
    m_samplingInterface = std::make_unique<cSamplingInterface>(m_scpiInterface,
                                                               m_settings->getSamplingSettings(),
                                                               ctrlFactory);

    // scpi interfaces
    m_systemInterface = std::make_unique<Com5003SystemInterface>(this, m_systemInfo.get(), m_senseInterface.get(), ctrlFactory);
    setScpiConnections(ScpiConnectionList{m_systemInterface.get(), m_samplingInterface.get(), m_senseInterface.get()});

    start();
}
