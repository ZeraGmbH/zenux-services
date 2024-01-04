#ifndef MOCKFORSENSEINTERFACECOM5003_H
#define MOCKFORSENSEINTERFACECOM5003_H

#include "i2csettings.h"
#include "mockpcbserver.h"
#include "com5003senseinterface.h"
#include "com5003systeminterface.h"
#include "com5003adjustment.h"
#include "atmelpermissionmock.h"
#include <memory>

class MockForSenseInterfaceCom5003  : public MockPcbServer
{
public:
    MockForSenseInterfaceCom5003(AtmelPermissionTemplate *permissionQueryHandler);
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }
    Com5003SenseInterface *getSenseInterface() { return m_senseInterface.get(); }
    Com5003Adjustment *getAdjustment() { return m_adjustment.get(); }
    cSenseSettings* getSenseSettings() { return m_senseSettings.get(); }
    cI2CSettings *getI2cSettings() { return m_i2cSettings.get(); }

private:
    std::unique_ptr<cI2CSettings> m_i2cSettings;
    std::unique_ptr<cSenseSettings> m_senseSettings;

    std::unique_ptr<Com5003SenseInterface> m_senseInterface;
    std::unique_ptr<Com5003SystemInterface> m_systemInterface; // for adj xml import/export

    std::unique_ptr<cSystemInfo> m_systemInfo;
    std::unique_ptr<Com5003Adjustment> m_adjustment;

    AtmelPermissionMock m_permissionMock;
};

#endif // MOCKFORSENSEINTERFACECOM5003_H
