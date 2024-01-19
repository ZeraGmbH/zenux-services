#ifndef DEMOSYSTEMCOM5003_H
#define DEMOSYSTEMCOM5003_H

#include "resmanrunfacade.h"
#include "mockcom5003dfull.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"

class DemoSystemCom5003
{
public:
    DemoSystemCom5003();
    virtual ~DemoSystemCom5003();
private:
    ResmanRunFacade *m_resman;
    MockCom5003dFull *m_mockcom5003d;
    MockSec1000d *m_mocksec1000d;
    MockZdsp1d *m_mockzdsp1d;
};

#endif // DEMOSYSTEMCOM5003_H
