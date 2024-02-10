#include "demoallservicescom5003.h"
#include "demofactoryi2cctrl.h"
#include "demoeventloopfeeder.h"
#include "demofactorydevicenodedsp.h"
#include "mockserverparamgenerator.h"

DemoAllServicesCom5003::DemoAllServicesCom5003()
{
    m_resman = new ResmanRunFacade;
    ServerParams params = MockServerParamGenerator::createParams("com5003d");
    m_mockcom5003d = new MockCom5003d(std::make_shared<DemoFactoryI2cCtrl>(std::make_unique<SettingsContainer>(params)));
    m_sec1000d = new MockSec1000d;
    m_zdsp1d = new MockZdsp1d(std::make_shared<DemoFactoryDeviceNodeDsp>());
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
