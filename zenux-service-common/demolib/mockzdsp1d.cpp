#include "mockzdsp1d.h"
#include "mockserverparamgenerator.h"

MockZdsp1d::MockZdsp1d(AbstractFactoryZdspSupportPtr zdspSupportFactory,
                       VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                       const QString &alternateConfigXml)
{
    const ServerParams params = MockServerParamGenerator::createParams("zdsp1d", alternateConfigXml);
    SettingsContainerPtr settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<ZDspServer>(
        std::move(settings),
        zdspSupportFactory,
        tcpNetworkFactory,
        false);
}

ZDspServer *MockZdsp1d::getServer()
{
    return m_server.get();
}
