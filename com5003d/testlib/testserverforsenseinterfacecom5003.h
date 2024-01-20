#ifndef TESTSERVERFORSENSEINTERFACECOM5003_H
#define TESTSERVERFORSENSEINTERFACECOM5003_H

#include "i2csettings.h"
#include "testpcbserver.h"
#include "com5003senseinterface.h"
#include "com5003systeminterface.h"
#include "abstractfactoryi2cctrl.h"
#include <memory>

class TestServerForSenseInterfaceCom5003  : public TestPcbServer
{
public:
    TestServerForSenseInterfaceCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory, bool systemInfoMock = false);
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }
    Com5003SenseInterface *getSenseInterface() { return m_senseInterface.get(); }
    cSenseSettings* getSenseSettings() { return m_senseSettings.get(); }
    I2cSettings *getI2cSettings() { return m_i2cSettings.get(); }
    SystemInfo* getSystemInfo() { return m_systemInfo.get(); }

private:
    std::unique_ptr<I2cSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;

    std::unique_ptr<Com5003SenseInterface> m_senseInterface;
    std::unique_ptr<Com5003SystemInterface> m_systemInterface;

    std::unique_ptr<SystemInfo> m_systemInfo;
};

#endif // TESTSERVERFORSENSEINTERFACECOM5003_H
