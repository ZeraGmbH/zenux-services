#include "mockmt310s2d.h"
#include "mockfactorydevicenodepcb.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"

MockMt310s2d::MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           const QString &serviceNameForAlternateDevice,
                           QString alternateConfigXml)
{
    MockI2cEEpromIoFactory::enableMock();

    ServerParams params = MockServerParamGenerator::createParams(serviceNameForAlternateDevice);
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    m_server = std::make_unique<cMT310S2dServer>(
        std::make_unique<SettingsContainer>(params),
        ctrlFactory,
        std::make_shared<MockFactoryDeviceNodePcb>(),
        tcpNetworkFactory,
        SettingsContainer::createChannelRangeFactory(serviceNameForAlternateDevice));
}
