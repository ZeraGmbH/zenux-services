#include "demosystemmt310s2.h"
#include <mockfactoryi2cctrl.h>
#include <demoeventloopfeeder.h>

DemoSystemMt310s2::DemoSystemMt310s2()
{
    m_resman = new ResmanRunFacade;
    DemoEventLoopFeeder::feedEventLoop();
    m_mockmt310s2d = new MockMt310s2d(std::make_shared<MockFactoryI2cCtrl>(true));
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
    DemoEventLoopFeeder::feedEventLoop();
}

DemoSystemMt310s2::~DemoSystemMt310s2()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockmt310s2d;
    DemoEventLoopFeeder::feedEventLoop();
    delete m_resman;
    DemoEventLoopFeeder::feedEventLoop();
}
