#include "testallservicescom5003.h"
#include <timemachineobject.h>

TestAllServicesCom5003::TestAllServicesCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resman = new ResmanRunFacade;
    TimeMachineObject::feedEventLoop();
    m_mockcom5003d = new MockCom5003d(ctrlFactory);
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
    TimeMachineObject::feedEventLoop();
}

TestAllServicesCom5003::~TestAllServicesCom5003()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockcom5003d;
    TimeMachineObject::feedEventLoop();
    delete m_resman;
    TimeMachineObject::feedEventLoop();
}
