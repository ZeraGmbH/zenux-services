#include "demoallservicesmt310s2.h"
#include <demofactoryi2cctrl.h>
#include <demoeventloopfeeder.h>

DemoAllServicesMt310s2::DemoAllServicesMt310s2()
{
    m_resman = new ResmanRunFacade;
    m_mockmt310s2d = new MockMt310s2d(std::make_shared<DemoFactoryI2cCtrl>());
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
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
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockmt310s2d;
    delete m_resman;
}
