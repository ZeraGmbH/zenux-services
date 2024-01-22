#include "demoallservicescom5003.h"
#include "demofactoryi2cctrl.h"

DemoAllServicesCom5003::DemoAllServicesCom5003()
{
    m_resman = new ResmanRunFacade;
    m_mockcom5003d = new MockCom5003d(std::make_shared<DemoFactoryI2cCtrl>());
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
}

DemoAllServicesCom5003::~DemoAllServicesCom5003()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockcom5003d;
    delete m_resman;
}
