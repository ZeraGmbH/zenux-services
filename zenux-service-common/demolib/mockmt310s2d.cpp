#include "mockmt310s2d.h"
#include "mockeepromi2cfactory.h"
#include "mockfactorydevicenodepcb.h"
#include "mockserverparamgenerator.h"
#include "controllerpersitentdata.h"
#include "simulsystemstatus.h"
#include "clampinterface.h"

MockMt310s2d::MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                           VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                           const QString &serviceNameForAlternateDevice,
                           const QString &alternateConfigXml) :
    m_ctrlFactory(ctrlFactory)
{
    connect(SimulSystemStatus::getInstance(), &SimulSystemStatus::sigHotplugDevChanged,
            this, &MockMt310s2d::onSimulGuiHotplugDevChanged);

    ServerParams params = MockServerParamGenerator::createParams(serviceNameForAlternateDevice);
    if(!alternateConfigXml.isEmpty())
        params.xmlFile = alternateConfigXml;
    m_server = std::make_unique<cMT310S2dServer>(
        std::make_unique<SettingsContainer>(params),
        ctrlFactory,
        std::make_shared<MockFactoryDeviceNodePcb>(),
        std::make_shared<MockEepromI2cFactory>(),
        tcpNetworkFactory,
        SettingsContainer::createChannelRangeFactory(serviceNameForAlternateDevice));
    setupHotplugChannelEnable();
}

void MockMt310s2d::fireHotplugInterruptControllerOnly(const QStringList &channelAliases)
{
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap infoMap;
    for (const QString &channelAlias : channelAliases)
        infoMap.insert(channelAlias, {"EMOB_MOCK-00V00", cClamp::undefined});
    fireHotplugInterrupt(infoMap);
}

void MockMt310s2d::fireHotplugInterrupt(const AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap &infoMap)
{
    cSenseSettings *senseSettings = m_server->getSenseSettings();
    quint16 interruptMask = 0;
    ControllerPersitentData::MuxChannelDeviceNameMap hotDevicesToSet;
    for(auto iter = infoMap.constBegin(); iter!= infoMap.constEnd(); iter++) {
        AbstractMockAllServices::hotplugI2cBus hotplugI2CBus = iter.value();
        QString channelAlias = iter.key();
        const SenseSystem::cChannelSettings* channelSetting = senseSettings->findChannelSettingByAlias1(channelAlias);
        if(channelSetting) {
            if(!hotplugI2CBus.controllerName.isEmpty()) {
                interruptMask |= (1 << channelSetting->m_nPluggedBit);
                hotDevicesToSet[channelSetting->m_nMuxChannelNo] = hotplugI2CBus;
            }
            if(cClamp::isValidClampType(hotplugI2CBus.clamp)) {
                // TODO
                interruptMask |= (1 << channelSetting->m_nPluggedBit);
            }
        }
    }
    ControllerPersitentData::setHotplugDevices(hotDevicesToSet);
    ControllerPersitentData::injectInterruptFlags(interruptMask);
    m_server->MTIntHandler(0);
}

HotPluggableControllerContainerPtr MockMt310s2d::getHotPluggableControllerContainer()
{
    return m_server->getHotPluggableControllerContainer();
}

cClampInterface *MockMt310s2d::getClampInterface() const
{
    return m_server->getClampInterface();
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
        fireHotplugInterruptControllerOnly(m_channelAliasesWithControllers.values());
}

void MockMt310s2d::setupHotplugChannelEnable()
{
    cSenseSettings *senseSettings = m_server->getSenseSettings();
    QVector<bool> enableMask(8);
    for (int i=0; i<simulChannelSequence.size(); ++i) {
        const QString &channelAlias = simulChannelSequence[i];
        const SenseSystem::cChannelSettings* channelSetting = senseSettings->findChannelSettingByAlias1(channelAlias);
        if (channelSetting != nullptr && channelSetting->m_nPluggedBit >= 0)
            enableMask[i] = true;
    }
    SimulSystemStatus::getInstance()->setChannelHotplugSupported(enableMask);
}
