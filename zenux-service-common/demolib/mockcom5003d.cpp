#include "mockcom5003d.h"
#include "mockfactorydevicenodepcb.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"

MockCom5003d::MockCom5003d(const QString &nameForConfigAndRanges,
                           AbstractFactoryI2cCtrlPtr ctrlFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           const QString &alternateConfigXml)
{
    MockI2cEEpromIoFactory::enableMock();

    ServerParams params = MockServerParamGenerator::createParams(nameForConfigAndRanges);
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    SettingsContainerPtr settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<cCOM5003dServer>(
        nameForConfigAndRanges,
        std::move(settings),
        ctrlFactory,
        std::make_shared<MockFactoryDeviceNodePcb>(),
        tcpNetworkFactory);
}
