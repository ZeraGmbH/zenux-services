#include "mocksec1000d.h"
#include "mockfactorydevicenodesec.h"
#include "mockserverparamgenerator.h"

MockSec1000d::MockSec1000d(VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           int ecUnitCount,
                           const QString &alternateConfigXml)
{
    ServerParams params = MockServerParamGenerator::createParams("sec1000d", alternateConfigXml);
    m_server = std::make_unique<cSEC1000dServer>(
        std::make_unique<SettingsContainer>(params),
        ecUnitCount,
        std::make_shared<MockFactoryDeviceNodeSec>(),
        tcpNetworkFactory);
}

cSEC1000dServer *MockSec1000d::getServer()
{
    return m_server.get();
}
