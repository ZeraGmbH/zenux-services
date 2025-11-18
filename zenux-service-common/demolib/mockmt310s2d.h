#ifndef MOCKMT310S2D_H
#define MOCKMT310S2D_H

#include "mt310s2d.h"
#include "abstractallservices.h"
#include "abstractfactoryi2cctrl.h"
#include <QSet>

class MockMt310s2d : public QObject
{
    Q_OBJECT
public:
    MockMt310s2d(AbstractFactoryI2cCtrlPtr ctrlFactory,
                 VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                 const QString &serviceNameForAlternateDevice,
                 const QString &alternateConfigXml = QString());
    void fireHotplugInterrupt(const QStringList &channelAliases);
    void fireHotplugInterruptControllerName(const AbstractAllServices::ChannelAliasHotplugDeviceNameMap &infoMap);

private slots:
    void onSimulGuiHotplugDevChanged(int channelIndex, bool active);
private:
    void setupHotplugChannelEnable();
    std::unique_ptr<cMT310S2dServer> m_server;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QSet<QString> m_channelAliasesWithControllers;
};

#endif // MOCKMT310S2D_H
