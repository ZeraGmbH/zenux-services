#include "testzdsp1dforvaraccess.h"

TestZdsp1dForVarAccess::TestZdsp1dForVarAccess(AbstractFactoryZdspSupportPtr zdspSupportFactory,
                                               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                               QString alternateConfigXml) :
    MockZdsp1d(zdspSupportFactory, tcpNetworkFactory, alternateConfigXml),
    m_testNetClient(tcpNetworkFactory)
{
}

ZdspClient *TestZdsp1dForVarAccess::createTestClient()
{
    return m_server->addClientForTest();
}
