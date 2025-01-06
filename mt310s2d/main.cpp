#include "mt310s2d.h"
#include "mt310s2dglobal.h"
#include "factoryi2cctrl.h"
#include "factorydevicenodepcb.h"
#include <tcpnetworkfactory.h>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);

    SettingsContainerPtr settings = std::make_unique<SettingsContainer>(cMT310S2dServer::defaultParams);
    std::shared_ptr<FactoryI2cCtrl> ctrlFactory = std::make_shared<FactoryI2cCtrl>(settings->getI2cSettings());
    cMT310S2dServer* mt310s2d = new cMT310S2dServer(
        std::move(settings),
        ctrlFactory,
        std::make_shared<FactoryDeviceNodePcb>(),
        VeinTcp::TcpNetworkFactory::create());
    qInfo(ServerName " started");

    int r = app->exec();

    delete mt310s2d;
    return (r);
}
