#ifndef MOCKMT310S2D_H
#define MOCKMT310S2D_H

#include "mt310s2d.h"
#include "abstractmockallservices.h"
#include "abstractfactoryi2cctrl.h"
#include <QMap>

class MockMt310s2d : public QObject
{
    Q_OBJECT
public:
    MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                 const QString &serviceNameForAlternateDevice,
                 const QString &alternateConfigXml = QString());

    void fireHotplugInterrupt(const AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap &deviceMap);
    void addStandardEmobControllers(const QStringList &channelAliases);
    void addClamps(const QList<AbstractMockAllServices::clampParam> &clampParams);
    void removeAllHotplugDevices();
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap getCurrentHotplugMap() const;
    HotPluggableControllerContainerPtr getHotPluggableControllerContainer();

    cClampInterface* getClampInterface() const;

private slots:
    void onSimulGuiHotplugDevChanged(int channelIndex, int deviceIndex);
private:
    void setupHotplugChannelEnable();
    std::unique_ptr<cMT310S2dServer> m_server;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    AbstractMockAllServices::ChannelAliasHotplugDeviceNameMap m_channelAliasesHotplugI2cBus;
};

#endif // MOCKMT310S2D_H
