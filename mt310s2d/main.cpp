#include "mt310s2d.h"
#include "mt310s2dglobal.h"
#include "factoryi2cctrl.h"
#include "factorydevicenodepcb.h"
#include <tcpnetworkfactory.h>
#include <QCoreApplication>
#include <QCommandLineParser>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);
    QCommandLineParser parser;
    const QCommandLineOption subdeviceparam("d", "device", "subdevice");
    parser.addOption(subdeviceparam);
    parser.process(*app);
    QString serviceName = "mt310s2d";
    if (parser.isSet(subdeviceparam))
        serviceName = parser.value(subdeviceparam);
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(serviceName);
    ServerParams defaultParams { ServerName,
                                 ServerVersion,
                                 "/etc/zera/mt310s2d/" + config.xsdFileName,
                                 "/etc/zera/mt310s2d/" + config.xmlFileName};

    SettingsContainerPtr settings = std::make_unique<SettingsContainer>(defaultParams);
    std::shared_ptr<FactoryI2cCtrl> ctrlFactory = std::make_shared<FactoryI2cCtrl>(settings->getI2cSettings());
    cMT310S2dServer* mt310s2d = new cMT310S2dServer(
        std::move(settings),
        ctrlFactory,
        std::make_shared<FactoryDeviceNodePcb>(),
        VeinTcp::TcpNetworkFactory::create(),
        SettingsContainer::createChannelRangeFactory(serviceName));
    qInfo(ServerName " started");

    int r = app->exec();

    delete mt310s2d;
    delete app;
    return (r);
}
