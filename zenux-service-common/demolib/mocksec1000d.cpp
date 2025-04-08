#include "mocksec1000d.h"
#include "mockfactorydevicenodesec.h"
#include "mockserverparamgenerator.h"

MockSec1000d::MockSec1000d(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           int ecUnitCount,
                           QString alternateConfigXml)
{
    ServerParams params = MockServerParamGenerator::createParams("sec1000d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;

    m_server = std::make_unique<cSEC1000dServer>(
        std::make_unique<SettingsContainer>(params),
        ecUnitCount,
        std::make_shared<MockFactoryDeviceNodeSec>(),
        tcpNetworkFactory);
}
