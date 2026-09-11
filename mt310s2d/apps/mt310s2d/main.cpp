#include "eeprom24lcxxxpca9547factory.h"
#include "mt310s2d.h"
#include "mt310s2dglobal.h"
#include "factoryi2cctrl.h"
#include "factorydevicenodepcb.h"
#include <tcpnetworkfactory.h>
#include <zenuxdeviceinfo.h>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);
    QString serviceName = "mt310s2d";
    QString deviceName = ZenuxDeviceInfo::getDeviceNameFromKernelParam();
    if (!deviceName.isEmpty())
        serviceName = deviceName +"d";
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceName);
    ServerParams defaultParams { 8,
                                 ServerName,
                                 ServerVersion,
                                 "/etc/zera/mt310s2d/" + config.xsdFileName,
                                 "/etc/zera/mt310s2d/" + config.xmlFileName};

    SettingsContainerPtr settings = std::make_unique<SettingsContainer>(defaultParams);
    AbstractFactoryI2cCtrlPtr ctrlFactory = std::make_shared<FactoryI2cCtrl>(settings->getI2cSettings());

    cMT310S2dServer* mt310s2d = new cMT310S2dServer(
        std::move(settings),
        ctrlFactory,
        std::make_shared<FactoryDeviceNodePcb>(),
        std::make_shared<Eeprom24LCxxxPca9547Factory>(),
        VeinTcp::TcpNetworkFactory::create(),
        SettingsContainer::createChannelRangeFactory(serviceName));
    qInfo(ServerName " started on %s", qPrintable(deviceName));

    int r = app->exec();

    delete mt310s2d;
    delete app;
    return (r);
}
