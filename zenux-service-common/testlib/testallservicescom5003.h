#ifndef TESTALLSERVICESCOM5003_H
#define TESTALLSERVICESCOM5003_H

#include "abstracttestallservices.h"
#include "resmanrunfacade.h"
#include "mockcom5003d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"
#include "testfactoryi2cctrl.h"

class TestAllServicesCom5003 : public AbstractTestAllServices
{
public:
    TestAllServicesCom5003(TestFactoryI2cCtrlPtr testCtrlFactory = std::make_shared<TestFactoryI2cCtrl>(true));
    TestAllServicesCom5003(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           TestFactoryI2cCtrlPtr testCtrlFactory = std::make_shared<TestFactoryI2cCtrl>(true));
    virtual ~TestAllServicesCom5003();

    void setRangeGetSetDelay(int rangeGetSetDelay) override;
    ZDspServer *getZdsp1dServer() override;
    cSEC1000dServer *getSecServer() override;

    void fireHotplugInterrupt(const ChannelAliasHotplugDeviceNameMap &deviceMap) override;
    ChannelAliasHotplugDeviceNameMap getCurrentHotplugMap() const override;
    cSenseSettingsPtr getSenseSettings() override;

    void addStandardEmobControllers(const QStringList &channelAliases) override;
    virtual void removeAllHotplugDevices() override;
    virtual void addClamps(const QList<clampParam> &clampParams) override;
private:
    void init(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory, TestFactoryI2cCtrlPtr ctrlFactory);
    void noHotplugMsg() const;

    ResmanRunFacade *m_resman;
    MockCom5003d *m_mockcom5003d;
    MockSec1000d *m_sec1000d;
    MockZdsp1d *m_zdsp1d;
    TestFactoryI2cCtrlPtr m_testCtrlFactory;
};

#endif // TESTALLSERVICESCOM5003_H
