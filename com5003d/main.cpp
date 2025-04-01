#include "com5003d.h"
#include "com5003dglobal.h"
#include "factorydevicenodepcb.h"
#include "factoryi2cctrl.h"
#include <tcpnetworkfactory.h>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);

    QString nameForConfigAndRanges = "com5003d";
    SettingsContainer::TServiceConfig config = SettingsContainer::getServiceConfig(nameForConfigAndRanges);
    ServerParams defaultParams { ServerName,
                               ServerVersion,
                               "/etc/zera/com5003d/" + config.xsdFileName,
                               "/etc/zera/com5003d/" + config.xmlFileName};


    SettingsContainerPtr settings = std::make_unique<SettingsContainer>(defaultParams);
    std::shared_ptr<FactoryI2cCtrl> ctrlFactory = std::make_shared<FactoryI2cCtrl>(settings->getI2cSettings());
    cCOM5003dServer* com5003d = new cCOM5003dServer(
        nameForConfigAndRanges,
        std::move(settings),
        ctrlFactory,
        std::make_shared<FactoryDeviceNodePcb>(),
        VeinTcp::TcpNetworkFactory::create());
    qInfo(ServerName " started");

    int r = app->exec();

    delete com5003d;
    delete app;
    return (r);
}
