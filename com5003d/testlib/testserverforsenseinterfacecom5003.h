#ifndef TESTSERVERFORSENSEINTERFACECOM5003_H
#define TESTSERVERFORSENSEINTERFACECOM5003_H

#include "i2csettings.h"
#include "samplinginterface.h"
#include "testpcbserver.h"
#include "senseinterfacecommon.h"
#include "com5003systeminterface.h"
#include "abstractfactoryi2cctrl.h"
#include <memory>

class TestServerForSenseInterfaceCom5003  : public TestPcbServer
{
public:
    TestServerForSenseInterfaceCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory,
                                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                       bool systemInfoMock = false);
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }
    SenseInterfaceCommon *getSenseInterface() { return m_senseInterface.get(); }
    cSenseSettings* getSenseSettings() { return m_senseSettings.get(); }
    I2cSettings *getI2cSettings() { return m_settings->getI2cSettings(); }
    SystemInfo* getSystemInfo() { return m_systemInfo.get(); }

private:
    std::unique_ptr<cSenseSettings> m_senseSettings;
    std::unique_ptr<SenseInterfaceCommon> m_senseInterface;
    std::unique_ptr<Com5003SystemInterface> m_systemInterface;
    std::unique_ptr<SystemInfo> m_systemInfo;
    std::unique_ptr<cSamplingInterface> m_samplingInterface;
};

#endif // TESTSERVERFORSENSEINTERFACECOM5003_H
