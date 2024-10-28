#include "demoallservicescom5003.h"
#include "demofactoryi2cctrl.h"
#include "demoeventloopfeeder.h"
#include "demofactorydevicenodedsp.h"
#include "mockserverparamgenerator.h"
#include <tcpworkerfactory.h>

DemoAllServicesCom5003::DemoAllServicesCom5003()
{
    // for now default to real network
    init(VeinTcp::TcpWorkerFactory::create());
}

DemoAllServicesCom5003::DemoAllServicesCom5003(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory)
{
    init(tcpWorkerFactory);
}

void DemoAllServicesCom5003::init(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory)
{
    m_resman = new ResmanRunFacade(tcpWorkerFactory);
    ServerParams params = MockServerParamGenerator::createParams("com5003d");
    m_mockcom5003d = new MockCom5003d(std::make_shared<DemoFactoryI2cCtrl>(std::make_unique<SettingsContainer>(params)), tcpWorkerFactory);
    m_sec1000d = new MockSec1000d(tcpWorkerFactory);
    m_zdsp1d = new MockZdsp1d(std::make_shared<DemoFactoryDeviceNodeDsp>(), tcpWorkerFactory);
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
