#ifndef MOCKSEC1000D_H
#define MOCKSEC1000D_H

#include "sec1000d.h"

class MockSec1000d
{
public:
    MockSec1000d(VeinTcp::AbstractTcpWorkerFactoryPtr tcpNetworkFactory,
                 QString alternateConfigXml = QString());
private:
    std::unique_ptr<cSEC1000dServer> m_server;
};

#endif // MOCKSEC1000D_H
