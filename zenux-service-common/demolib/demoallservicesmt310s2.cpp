#include "demoallservicesmt310s2.h"
#include "demofactoryi2cctrl.h"
#include "demoeventloopfeeder.h"
#include "demofactorydevicenodedsp.h"
#include "mockserverparamgenerator.h"
#include "autojournalloggerfacade.h"
#include <tcpworkerfactory.h>

DemoAllServicesMt310s2::DemoAllServicesMt310s2()
{
    // for now default to real network
    init(VeinTcp::TcpWorkerFactory::create());
}

DemoAllServicesMt310s2::DemoAllServicesMt310s2(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory)
{
    init(tcpWorkerFactory);
}

void DemoAllServicesMt310s2::init(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory)
{
    m_autoLogger = new AutoJournalLoggerFacade;
    m_resman = new ResmanRunFacade(tcpWorkerFactory);
    ServerParams params = MockServerParamGenerator::createParams("mt310s2d");
    m_mt310s2d = new MockMt310s2d(std::make_shared<DemoFactoryI2cCtrl>(std::make_unique<SettingsContainer>(params)), tcpWorkerFactory);
    m_sec1000d = new MockSec1000d(tcpWorkerFactory);
    m_zdsp1d = new MockZdsp1d(std::make_shared<DemoFactoryDeviceNodeDsp>(), tcpWorkerFactory);
    DemoEventLoopFeeder::feedEventLoop();
#ifdef GUI_SIMULATION
    m_gui = new SimulQmlGui;
#endif
}

DemoAllServicesMt310s2::~DemoAllServicesMt310s2()
{
#ifdef GUI_SIMULATION
    delete m_gui;
#endif
    delete m_zdsp1d;
    delete m_sec1000d;
    delete m_mt310s2d;
    delete m_resman;
    delete m_autoLogger;
}
