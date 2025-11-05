#ifndef MOCKMT310S2D_H
#define MOCKMT310S2D_H

#include "mt310s2d.h"
#include "abstractfactoryi2cctrl.h"

class MockMt310s2d
{
public:
    MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                 const QString &serviceNameForAlternateDevice,
                 QString alternateConfigXml = QString());
    void fireHotplugInterrupt(const QStringList &channelAliases);
private:
    std::unique_ptr<cMT310S2dServer> m_server;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
};

#endif // MOCKMT310S2D_H
