#ifndef DEMOALLSERVICESCOM5003_H
#define DEMOALLSERVICESCOM5003_H

#include "abstractmockallservices.h"
#include "resmanrunfacade.h"
#include "mockcom5003d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"

class DemoAllServicesCom5003 : public AbstractMockAllServices
{
public:
    DemoAllServicesCom5003();
    virtual ~DemoAllServicesCom5003();
private:
    ResmanRunFacade *m_resman;
    MockCom5003d *m_mockcom5003d;
    MockSec1000d *m_mocksec1000d;
    MockZdsp1d *m_mockzdsp1d;
};

#endif // DEMOALLSERVICESCOM5003_H
