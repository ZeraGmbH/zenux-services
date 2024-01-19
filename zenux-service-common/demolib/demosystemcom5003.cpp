#include "demosystemcom5003.h"
#include "mockfactoryi2cctrl.h"
#include <demoeventloopfeeder.h>

DemoSystemCom5003::DemoSystemCom5003()
{
    m_resman = new ResmanRunFacade;
    DemoEventLoopFeeder::feedEventLoop();
    m_mockcom5003d = new MockCom5003dFull(std::make_shared<MockFactoryI2cCtrl>(true));
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
    DemoEventLoopFeeder::feedEventLoop();
}

DemoSystemCom5003::~DemoSystemCom5003()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockcom5003d;
    DemoEventLoopFeeder::feedEventLoop();
    delete m_resman;
    DemoEventLoopFeeder::feedEventLoop();
}
