#include "testallservicescom5003.h"
#include <timemachineobject.h>
#include "testfactorydevicenodedsp.h"

TestAllServicesCom5003::TestAllServicesCom5003(AbstractFactoryI2cCtrlPtr ctrlFactory)
{
    m_resman = new ResmanRunFacade;
    TimeMachineObject::feedEventLoop();
    m_mockcom5003d = new MockCom5003d(ctrlFactory);
    m_sec1000d = new MockSec1000d;
    m_zdsp1d = new MockZdsp1d(std::make_shared<TestFactoryDeviceNodeDsp>());
    TimeMachineObject::feedEventLoop();
}

TestAllServicesCom5003::~TestAllServicesCom5003()
{
    delete m_zdsp1d;
    delete m_sec1000d;
    delete m_mockcom5003d;
    TimeMachineObject::feedEventLoop();
    delete m_resman;
    TimeMachineObject::feedEventLoop();
}
