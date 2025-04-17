#ifndef DSPINITFORTEST_H
#define DSPINITFORTEST_H

#include "proxyclientfortest.h"
#include "dspinterface.h"

class DspInitForTest
{
public:
    DspInitForTest();
    Zera::DspInterfacePtr getDspInterface();
    Zera::ProxyClientForTestPtr getProxyClient();
private:
    Zera::DspInterfacePtr m_dspInterface;
    Zera::ProxyClientForTestPtr m_proxyClient;
};

#endif // DSPINITFORTEST_H
