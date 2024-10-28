#ifndef MOCKMT310S2D_H
#define MOCKMT310S2D_H

#include "mt310s2d.h"

class MockMt310s2d
{
public:
    MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                 QString alternateConfigXml = QString());
private:
    std::unique_ptr<cMT310S2dServer> m_server;
};

#endif // MOCKMT310S2D_H
