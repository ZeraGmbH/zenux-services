#ifndef TESTSERVERFORSENSEINTERFACEMT310S2_H
#define TESTSERVERFORSENSEINTERFACEMT310S2_H

#include "testpcbserver.h"
#include "senseinterfacecommon.h"
#include "mt310s2systeminterface.h"
#include "clampinterface.h"
#include "samplinginterface.h"
#include "abstractfactoryi2cctrl.h"

class TestServerForSenseInterfaceMt310s2 : public TestPcbServer
{
public:
    TestServerForSenseInterfaceMt310s2(AbstractFactoryI2cCtrlPtr ctrlFactory,
                                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                       const QString &serviceNameForAlternateDevice = "mt310s2",
                                       bool systemInfoMock = false);
    QString getDeviceVersion() { return m_systemInfo->getDeviceVersion(); }

    SenseInterfaceCommon *getSenseInterface() { return m_senseInterface.get(); }
    cClampInterface *getClampInterface() { return m_clampInterface.get(); }
    Mt310s2SystemInfo *getSystemInfo() { return m_systemInfo.get(); }

    cClamp *addClamp(int clampType, QString channelAlias);
    void removeAllClamps();

private:
    std::unique_ptr<Mt310s2SystemInfo> m_systemInfo;
    std::unique_ptr<SenseInterfaceCommon> m_senseInterface;
    std::unique_ptr<cClampInterface> m_clampInterface;
    std::unique_ptr<Mt310s2SystemInterface> m_systemInterface;
    std::unique_ptr<cSamplingInterface> m_samplingInterface;
};

#endif // TESTSERVERFORSENSEINTERFACEMT310S2_H
