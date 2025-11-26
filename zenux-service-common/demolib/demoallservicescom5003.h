#ifndef DEMOALLSERVICESCOM5003_H
#define DEMOALLSERVICESCOM5003_H

#include "abstractmockallservices.h"
#include "resmanrunfacade.h"
#include "mockcom5003d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"
#ifdef GUI_SIMULATION
#include "simulqmlgui.h"
#endif

class DemoAllServicesCom5003 : public AbstractMockAllServices
{
public:
    DemoAllServicesCom5003();
    explicit DemoAllServicesCom5003(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    virtual ~DemoAllServicesCom5003() override;

    void fireHotplugInterrupt(const ChannelAliasHotplugDeviceNameMap &deviceMap) override;
    ChannelAliasHotplugDeviceNameMap getCurrentHotplugMap() const override;

    void addStandardEmobControllers(const QStringList &channelAliases) override;
    virtual void removeAllHotplugDevices() override;
    virtual void addClamps(const QList<clampParam> &clampParams) override;
private:
    void init(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory);
    void noHotplugMsg() const;

    ResmanRunFacade *m_resman;
    MockCom5003d *m_mockcom5003d;
    MockSec1000d *m_sec1000d;
    MockZdsp1d *m_zdsp1d;
#ifdef GUI_SIMULATION
    SimulQmlGui *m_gui;
#endif
};

#endif // DEMOALLSERVICESCOM5003_H
