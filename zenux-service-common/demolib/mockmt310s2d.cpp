#include "mockmt310s2d.h"
#include "mockfactorydevicenodepcb.h"
#include "mockserverparamgenerator.h"
#include "mocki2ceepromiofactory.h"
#include "controllerpersitentdata.h"
#include "simulsystemstatus.h"

MockMt310s2d::MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           const QString &serviceNameForAlternateDevice,
                           QString alternateConfigXml) :
    m_ctrlFactory(ctrlFactory)
{
    MockI2cEEpromIoFactory::enableMock();
    connect(SimulSystemStatus::getInstance(), &SimulSystemStatus::sigHotplugDevChanged,
            this, &MockMt310s2d::onSimulGuiHotplugDevChanged);

    ServerParams params = MockServerParamGenerator::createParams(serviceNameForAlternateDevice);
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    m_server = std::make_unique<cMT310S2dServer>(
        std::make_unique<SettingsContainer>(params),
        ctrlFactory,
        std::make_shared<MockFactoryDeviceNodePcb>(),
        tcpNetworkFactory,
        SettingsContainer::createChannelRangeFactory(serviceNameForAlternateDevice));
    setupHotplugChannelEnable();
}

void MockMt310s2d::fireHotplugInterrupt(const QStringList &channelAliases)
{
    cSenseSettings *senseSettings = m_server->getSenseSettings();
    quint16 interruptMask = 0;
    SenseSystem::cChannelSettings* channelSetting;
    for (const QString &channelAlias : channelAliases) {
        channelSetting = senseSettings->findChannelSettingByAlias1(channelAlias);
        interruptMask |= (1 << channelSetting->m_nPluggedBit);
    }
    ControllerPersitentData::injectInterruptFlags(interruptMask);
    m_server->MTIntHandler(0);
}

void MockMt310s2d::fireHotplugInterruptControllerName(const AbstractAllServices::ChannelAliasHotplugDeviceNameMap &infoMap)
{
    cSenseSettings *senseSettings = m_server->getSenseSettings();
    quint16 interruptMask = 0;
    SenseSystem::cChannelSettings* channelSetting;
    ControllerPersitentData::MuxChannelDeviceNameMap hotDevicesToSet;
    for (const QString &channelAlias : infoMap.keys()) {
        channelSetting = senseSettings->findChannelSettingByAlias1(channelAlias);
        interruptMask |= (1 << channelSetting->m_nPluggedBit);
        hotDevicesToSet[channelSetting->m_nMuxChannelNo] = infoMap.value(channelAlias);
    }
    ControllerPersitentData::setHotplugDevices(hotDevicesToSet);
    ControllerPersitentData::injectInterruptFlags(interruptMask);
    m_server->MTIntHandler(0);
}

static const QStringList simulChannelSequence = QStringList() << "UL1" << "UL2" << "UL3" << "UAUX" << "IL1" << "IL2" << "IL3" << "IAUX";

void MockMt310s2d::onSimulGuiHotplugDevChanged(int channelIndex, bool active)
{
    const QString channelAlias = simulChannelSequence[channelIndex];
    bool change = false;
    if(active) {
        change = !m_channelAliasesWithControllers.contains(channelAlias);
        m_channelAliasesWithControllers.insert(channelAlias);
    }
    else {
        change = m_channelAliasesWithControllers.contains(channelAlias);
        m_channelAliasesWithControllers.remove(channelAlias);
    }
    if(change)
        fireHotplugInterrupt(m_channelAliasesWithControllers.values());
}

void MockMt310s2d::setupHotplugChannelEnable()
{
    cSenseSettings *senseSettings = m_server->getSenseSettings();
    QVector<bool> enableMask(8);
    for (int i=0; i<simulChannelSequence.size(); ++i) {
        const QString &channelAlias = simulChannelSequence[i];
        SenseSystem::cChannelSettings* channelSetting = senseSettings->findChannelSettingByAlias1(channelAlias);
        if (channelSetting != nullptr && channelSetting->m_nPluggedBit >= 0)
            enableMask[i] = true;
    }
    SimulSystemStatus::getInstance()->setChannelHotplugSupported(enableMask);
}
