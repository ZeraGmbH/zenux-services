#ifndef MOCKFORSENSEINTERFACECOM5003_H
#define MOCKFORSENSEINTERFACECOM5003_H

#include "i2csettings.h"
#include "mockpcbserver.h"
#include "com5003senseinterface.h"
#include "com5003systeminterface.h"
#include "atmelctrlfactoryinterface.h"
#include <memory>

class MockForSenseInterfaceCom5003  : public MockPcbServer
{
public:
    MockForSenseInterfaceCom5003(AtmelCtrlFactoryInterfacePrt ctrlFactory, bool systemInfoMock = false);
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }
    Com5003SenseInterface *getSenseInterface() { return m_senseInterface.get(); }
    cSenseSettings* getSenseSettings() { return m_senseSettings.get(); }
    cI2CSettings *getI2cSettings() { return m_i2cSettings.get(); }
    SystemInfo* getSystemInfo() { return m_systemInfo.get(); }

private:
    std::unique_ptr<cI2CSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;

    std::unique_ptr<Com5003SenseInterface> m_senseInterface;
    std::unique_ptr<Com5003SystemInterface> m_systemInterface;

    std::unique_ptr<SystemInfo> m_systemInfo;
};

#endif // MOCKFORSENSEINTERFACECOM5003_H
