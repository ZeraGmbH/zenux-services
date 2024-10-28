#ifndef DEMOALLSERVICESMT310S2_H
#define DEMOALLSERVICESMT310S2_H

#include "abstractmockallservices.h"
#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "mocksec1000d.h"
#include "mockzdsp1d.h"
#ifdef GUI_SIMULATION
#include "simulqmlgui.h"
#endif

class AutoJournalLoggerFacade;

class DemoAllServicesMt310s2 : public AbstractMockAllServices
{
public:
    DemoAllServicesMt310s2();
    DemoAllServicesMt310s2(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory);
    virtual ~DemoAllServicesMt310s2();

private:
    void init(VeinTcp::AbstractTcpWorkerFactoryPtr tcpWorkerFactory);
    AutoJournalLoggerFacade *m_autoLogger;
    ResmanRunFacade *m_resman;
    MockMt310s2d *m_mt310s2d;
    MockSec1000d *m_sec1000d;
    MockZdsp1d *m_zdsp1d;
#ifdef GUI_SIMULATION
    SimulQmlGui *m_gui;
#endif
};

#endif // DEMOALLSERVICESMT310S2_H
