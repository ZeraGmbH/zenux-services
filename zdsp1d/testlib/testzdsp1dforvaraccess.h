#ifndef TESTZDSP1DFORVARACCESS_H
#define TESTZDSP1DFORVARACCESS_H

#include <mockzdsp1d.h>

class TestZdsp1dForVarAccess : public MockZdsp1d
{
public:
    TestZdsp1dForVarAccess(AbstractFactoryDeviceNodeDspPtr deviceNodeFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           QString alternateConfigXml = QString());
    cZDSP1Client* createTestClient();
private:
    VeinTcp::TcpPeer m_testNetClient;
};

#endif // TESTZDSP1DFORVARACCESS_H
