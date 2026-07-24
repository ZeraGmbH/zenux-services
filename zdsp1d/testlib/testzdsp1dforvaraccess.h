#ifndef TESTZDSP1DFORVARACCESS_H
#define TESTZDSP1DFORVARACCESS_H

#include <mockzdsp1d.h>

class TestZdsp1dForVarAccess : public MockZdsp1d
{
public:
    TestZdsp1dForVarAccess(const AbstractFactoryZdspSupportPtr &zdspSupportFactory,
                           const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                           const QString &alternateConfigXml = QString());
    ZdspClient* createTestClient();
private:
    VeinTcp::TcpPeer m_testNetClient;
};

#endif // TESTZDSP1DFORVARACCESS_H
