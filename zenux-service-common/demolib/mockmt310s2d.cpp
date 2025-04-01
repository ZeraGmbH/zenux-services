#include "mockmt310s2d.h"
#include "mockfactorydevicenodepcb.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"

MockMt310s2d::MockMt310s2d(const QString &nameForConfigAndRanges,
                           AbstractFactoryI2cCtrlPtr ctrlFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           const QString &alternateConfigXml)
{
    MockI2cEEpromIoFactory::enableMock();

    ServerParams params = MockServerParamGenerator::createParams(nameForConfigAndRanges);
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    m_server = std::make_unique<cMT310S2dServer>(
        nameForConfigAndRanges,
        std::make_unique<SettingsContainer>(params),
        ctrlFactory,
        std::make_shared<MockFactoryDeviceNodePcb>(),
        tcpNetworkFactory);
}
