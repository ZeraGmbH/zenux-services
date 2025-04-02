#ifndef MOCKCOM5003D_H
#define MOCKCOM5003D_H

#include "com5003d.h"

class MockCom5003d
{
public:
    MockCom5003d(const QString& nameForConfigAndRanges,
                 AbstractFactoryI2cCtrlPtr ctrlFactory,
                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                 const QString &alternateConfigXml = QString());
private:
    std::unique_ptr<cCOM5003dServer> m_server;
};

#endif // MOCKCOM5003D_H
