#include "testallservicesmt310s2.h"
#include <timemachineobject.h>

TestAllServicesMt310s2::TestAllServicesMt310s2(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resman = new ResmanRunFacade;
    TimeMachineObject::feedEventLoop();;
    m_mockmt310s2d = new MockMt310s2d(ctrlFactory);
    m_mocksec1000d = new MockSec1000d;
    m_mockzdsp1d = new MockZdsp1d;
    TimeMachineObject::feedEventLoop();
}

TestAllServicesMt310s2::~TestAllServicesMt310s2()
{
    delete m_mockzdsp1d;
    delete m_mocksec1000d;
    delete m_mockmt310s2d;
    TimeMachineObject::feedEventLoop();
    delete m_resman;
    TimeMachineObject::feedEventLoop();
}
