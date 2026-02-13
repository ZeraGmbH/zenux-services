#ifndef MOCKSEC1000D_H
#define MOCKSEC1000D_H

#include "sec1000d.h"

class MockSec1000d
{
public:
    MockSec1000d(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                 int ecUnitCount,
                 const QString &alternateConfigXml = QString());
    cSEC1000dServer* getServer();
private:
    std::unique_ptr<cSEC1000dServer> m_server;
};

#endif // MOCKSEC1000D_H
