#include "mockcom5003d.h"
#include "mockeepromi2cfactory.h"
#include "mockfactorydevicenodepcb.h"
#include "mockserverparamgenerator.h"

MockCom5003d::MockCom5003d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           const QString &alternateConfigXml)
{
    ServerParams params = MockServerParamGenerator::createParams("com5003d");
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    SettingsContainerPtr settings = std::make_unique<SettingsContainer>(params);
    m_server = std::make_unique<cCOM5003dServer>(
        std::move(settings),
        ctrlFactory,
        std::make_shared<MockFactoryDeviceNodePcb>(),
        std::make_shared<MockEepromI2cFactory>(),
        tcpNetworkFactory,
        SettingsContainer::createChannelRangeFactory("com5003d"));
}
