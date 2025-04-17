#include "dspinitfortest.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>

DspInitForTest::DspInitForTest()
{
    m_dspInterface =  std::make_shared<Zera::cDSPInterface>();
    m_proxyClient = Zera::ProxyClientForTest::create();
    m_dspInterface->setClientSmart(m_proxyClient);
    TimeMachineForTest::reset();
    TimerFactoryQtForTest::enableTest();
}

Zera::DspInterfacePtr DspInitForTest::getDspInterface()
{
    return m_dspInterface;
}

Zera::ProxyClientForTestPtr DspInitForTest::getProxyClient()
{
    return m_proxyClient;
}
