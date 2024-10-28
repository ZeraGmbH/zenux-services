#ifndef TESTSERVERFORSENSEINTERFACEMT310S2_H
#define TESTSERVERFORSENSEINTERFACEMT310S2_H

#include "testpcbserver.h"
#include "senseinterfacecommon.h"
#include "mt310s2systeminterface.h"
#include "clampinterface.h"
#include "abstractfactoryi2cctrl.h"

class TestServerForSenseInterfaceMt310s2 : public TestPcbServer
{
public:
    TestServerForSenseInterfaceMt310s2(AbstractFactoryI2cCtrlPtr ctrlFactory,
                                       VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory,
                                       bool systemInfoMock = false);
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }

    SenseInterfaceCommon *getSenseInterface() { return m_senseInterface.get(); }
    cClampInterface *getClampInterface() { return m_clampInterface.get(); }
    cSenseSettings *getSenseSettings() { return m_senseSettings.get(); }
    I2cSettings *getI2cSettings() { return m_settings->getI2cSettings(); }
    Mt310s2SystemInfo *getSystemInfo() { return m_systemInfo.get(); }

    cClamp *addClamp(int clampTypeNo, QString channelAlias1);
    void removeAllClamps();

private:
    std::unique_ptr<cSenseSettings> m_senseSettings;
    
    std::unique_ptr<Mt310s2SystemInfo> m_systemInfo;

    std::unique_ptr<SenseInterfaceCommon> m_senseInterface;
    std::unique_ptr<cClampInterface> m_clampInterface;

    std::unique_ptr<Mt310s2SystemInterface> m_systemInterface;
};

#endif // TESTSERVERFORSENSEINTERFACEMT310S2_H
