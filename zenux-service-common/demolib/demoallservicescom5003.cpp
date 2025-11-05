#include "demoallservicescom5003.h"
#include "demofactoryi2cctrl.h"
#include "demoeventloopfeeder.h"
#include "demofactorydevicenodedsp.h"
#include "mockserverparamgenerator.h"
#include <tcpnetworkfactory.h>

DemoAllServicesCom5003::DemoAllServicesCom5003()
{
    // for now default to real network
    init(VeinTcp::TcpNetworkFactory::create());
}

DemoAllServicesCom5003::DemoAllServicesCom5003(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory)
{
    init(tcpNetworkFactory);
}

void DemoAllServicesCom5003::init(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory)
{
    m_resman = new ResmanRunFacade(tcpNetworkFactory);
    ServerParams params = MockServerParamGenerator::createParams("com5003d");
    m_mockcom5003d = new MockCom5003d(std::make_shared<DemoFactoryI2cCtrl>(std::make_unique<SettingsContainer>(params)), tcpNetworkFactory);
    m_sec1000d = new MockSec1000d(tcpNetworkFactory, cSEC1000dServer::Com5003EcUnitCount);
    m_zdsp1d = new MockZdsp1d(std::make_shared<DemoFactoryDeviceNodeDsp>(), tcpNetworkFactory);
    DemoEventLoopFeeder::feedEventLoop();
#ifdef GUI_SIMULATION
    m_gui = new SimulQmlGui;
#endif
}

DemoAllServicesCom5003::~DemoAllServicesCom5003()
{
#ifdef GUI_SIMULATION
    delete m_gui;
#endif
    delete m_zdsp1d;
    delete m_sec1000d;
    delete m_mockcom5003d;
    delete m_resman;
}

void DemoAllServicesCom5003::fireHotplugInterrupt(const QStringList &channelAliases)
{
    Q_UNUSED(channelAliases)
    qWarning("Hotplug not supported on COM5003 yet!");
}
