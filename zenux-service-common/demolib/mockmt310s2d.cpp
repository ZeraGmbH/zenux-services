#include "mockmt310s2d.h"
#include "mockfactorydevicenodepcb.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"
#include "controllerpersitentdata.h"

MockMt310s2d::MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           const QString &serviceNameForAlternateDevice,
                           QString alternateConfigXml) :
    m_ctrlFactory(ctrlFactory)
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

void MockMt310s2d::fireHotplugInterrupt(const QStringList &channelAliases)
{
    cSenseSettings *senseSettings = m_server->getSenseSettings();
    quint16 interruptMask = 0;
    for (const QString &channelAlias : channelAliases) {
        SenseSystem::cChannelSettings* channelSetting = senseSettings->findChannelSettingByAlias1(channelAlias);
        interruptMask |= (1 << channelSetting->m_nPluggedBit);
    }
    ControllerPersitentData::injectInterruptFlags(interruptMask);
    m_server->MTIntHandler(0);
}
