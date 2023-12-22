#ifndef MOCKFORSENSEINTERFACEMT310S2_H
#define MOCKFORSENSEINTERFACEMT310S2_H

#include "mockpcbserver.h"
#include "mt310s2senseinterface.h"
#include "clampinterface.h"

class MockForSenseInterfaceMt310s2 : public MockPcbServer
{
public:
    MockForSenseInterfaceMt310s2();
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }
    Mt310s2SenseInterface* getSenseInterface() { return m_senseInterface.get(); }
    cClampInterface* getClampInterface() { return m_clampInterface.get(); }
    cSenseSettings* getSenseSettings() { return m_senseSettings.get(); }
private:
    std::unique_ptr<cI2CSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;
    std::unique_ptr<cSystemInfo> m_systemInfo;
    std::unique_ptr<Mt310s2SenseInterface> m_senseInterface;
    std::unique_ptr<cClampInterface> m_clampInterface;
};

#endif // MOCKFORSENSEINTERFACEMT310S2_H
