#ifndef MOCKMT310S2D_H
#define MOCKMT310S2D_H

#include "mt310s2d.h"
#include "mockhotpluggablecontrollercontainer.h"

class MockMt310s2d
{
public:
    MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                 const QString &serviceNameForAlternateDevice,
                 QString alternateConfigXml = QString());
    MockHotPluggableControllerContainerPtr getHotPluggableControllerContainer();
private:
    std::unique_ptr<cMT310S2dServer> m_server;
    MockHotPluggableControllerContainerPtr m_hotPluggableControllerContainer;
};

#endif // MOCKMT310S2D_H
