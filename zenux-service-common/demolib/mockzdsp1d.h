#ifndef MOCKZDSP1D_H
#define MOCKZDSP1D_H

#include "zdspserver.h"

class MockZdsp1d
{
public:
    MockZdsp1d(AbstractFactoryZdspSupportPtr zdspSupportFactory,
               VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
               const QString &alternateConfigXml = QString());
    // As long as there is no TestZdsp1d
    ZDspServer* getServer();
protected:
    std::unique_ptr<ZDspServer> m_server;
};

#endif // MOCKZDSP1D_H
