#include "demoallservicescom5003.h"
#include "demofactoryi2cctrl.h"
#include <demoeventloopfeeder.h>

DemoAllServicesCom5003::DemoAllServicesCom5003()
{
    m_resman = new ResmanRunFacade;
    DemoEventLoopFeeder::feedEventLoop();
    m_mockcom5003d = new MockCom5003d(std::make_shared<DemoFactoryI2cCtrl>(true));
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
    DemoEventLoopFeeder::feedEventLoop();
}

DemoAllServicesCom5003::~DemoAllServicesCom5003()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockcom5003d;
    DemoEventLoopFeeder::feedEventLoop();
    delete m_resman;
    DemoEventLoopFeeder::feedEventLoop();
}
