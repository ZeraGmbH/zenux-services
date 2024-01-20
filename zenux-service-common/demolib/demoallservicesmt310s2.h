#ifndef DEMOALLSERVICESMT310S2_H
#define DEMOALLSERVICESMT310S2_H

#include "abstractmockallservices.h"
#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"

class DemoAllServicesMt310s2 : public AbstractMockAllServices
{
public:
    DemoAllServicesMt310s2();
    virtual ~DemoAllServicesMt310s2();
private:
    ResmanRunFacade *m_resman;
    MockMt310s2d *m_mockmt310s2d;
    MockSec1000d *m_mocksec1000d;
    MockZdsp1d *m_mockzdsp1d;
};

#endif // DEMOALLSERVICESMT310S2_H
