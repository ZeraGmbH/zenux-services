#include "mockzdsp1d.h"
#include "mockserverparamgenerator.h"

MockZdsp1d::MockZdsp1d(AbstractFactoryDeviceNodeDspPtr deviceNodeFactory,
                       VeinTcp::AbstractTcpWorkerFactoryPtr tcpNetworkFactory,
                       QString alternateConfigXml)
{
    ServerParams params = MockServerParamGenerator::createParams("zdsp1d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    SettingsContainerPtr settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<ZDspServer>(std::move(settings), deviceNodeFactory, tcpNetworkFactory);
}
