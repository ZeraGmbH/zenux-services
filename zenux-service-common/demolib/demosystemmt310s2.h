#ifndef DEMOSYSTEMMT310S2_H
#define DEMOSYSTEMMT310S2_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"

class DemoSystemMt310s2
{
public:
    DemoSystemMt310s2();
    virtual ~DemoSystemMt310s2();
private:
    ResmanRunFacade *m_resman;
    MockMt310s2d *m_mockmt310s2d;
    MockSec1000d *m_mocksec1000d;
    MockZdsp1d *m_mockzdsp1d;
};

#endif // DEMOSYSTEMMT310S2_H
