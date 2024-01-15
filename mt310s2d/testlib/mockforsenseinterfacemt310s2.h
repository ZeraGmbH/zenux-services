#ifndef MOCKFORSENSEINTERFACEMT310S2_H
#define MOCKFORSENSEINTERFACEMT310S2_H

#include "mockpcbserver.h"
#include "mt310s2senseinterface.h"
#include "mt310s2systeminterface.h"
#include "clampinterface.h"
#include "factorycontrollerabstract.h"

class MockForSenseInterfaceMt310s2 : public MockPcbServer
{
public:
    MockForSenseInterfaceMt310s2(FactoryControllerAbstractPtr ctrlFactory, bool systemInfoMock = false);
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }

    Mt310s2SenseInterface *getSenseInterface() { return m_senseInterface.get(); }
    cClampInterface *getClampInterface() { return m_clampInterface.get(); }
    cSenseSettings *getSenseSettings() { return m_senseSettings.get(); }
    cI2CSettings *getI2cSettings() { return m_i2cSettings.get(); }
    Mt310s2SystemInfo *getSystemInfo() { return m_systemInfo.get(); }

    void addClamp(int clampTypeNo, QString channelAlias1);
    void removeAllClamps();

private:
    std::unique_ptr<cI2CSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;
    
    std::unique_ptr<Mt310s2SystemInfo> m_systemInfo;

    std::unique_ptr<Mt310s2SenseInterface> m_senseInterface;
    std::unique_ptr<cClampInterface> m_clampInterface;

    std::unique_ptr<Mt310s2SystemInterface> m_systemInterface;
};

#endif // MOCKFORSENSEINTERFACEMT310S2_H
