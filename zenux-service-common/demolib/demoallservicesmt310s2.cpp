#include "demoallservicesmt310s2.h"
#include "demofactoryi2cctrl.h"
#include "demoeventloopfeeder.h"
#include "demofactorydevicenodedsp.h"

DemoAllServicesMt310s2::DemoAllServicesMt310s2()
{
    m_resman = new ResmanRunFacade;
    m_mt310s2d = new MockMt310s2d(std::make_shared<DemoFactoryI2cCtrl>());
    m_sec1000d = new MockSec1000d;
    m_zdsp1d = new MockZdsp1d(std::make_shared<DemoFactoryDeviceNodeDsp>());
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
}
