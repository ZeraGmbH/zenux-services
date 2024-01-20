#ifndef TESTALLSERVICESCOM5003_H
#define TESTALLSERVICESCOM5003_H

#include "abstractmockallservices.h"
#include "resmanrunfacade.h"
#include "mockcom5003d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"

class TestAllServicesCom5003 : public AbstractMockAllServices
{
public:
    TestAllServicesCom5003();
    virtual ~TestAllServicesCom5003();
private:
    ResmanRunFacade *m_resman;
    MockCom5003d *m_mockcom5003d;
    MockSec1000d *m_mocksec1000d;
    MockZdsp1d *m_mockzdsp1d;
};

#endif // TESTALLSERVICESCOM5003_H
