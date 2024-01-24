#include "testallservicescom5003.h"
#include <demoeventloopfeeder.h>

TestAllServicesCom5003::TestAllServicesCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resman = new ResmanRunFacade;
    DemoEventLoopFeeder::feedEventLoop();
    m_mockcom5003d = new MockCom5003d(ctrlFactory);
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
    DemoEventLoopFeeder::feedEventLoop();
}

TestAllServicesCom5003::~TestAllServicesCom5003()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockcom5003d;
    DemoEventLoopFeeder::feedEventLoop();
    delete m_resman;
    DemoEventLoopFeeder::feedEventLoop();
}
