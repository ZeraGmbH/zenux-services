#include "zdspserver.h"
#include "factorydevicenodedsp.h"
#include <tcpnetworkfactory.h>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);

    ZDspServer* zdsp1d = new ZDspServer(
        std::make_unique<SettingsContainer>(ZDspServer::defaultParams),
        std::make_shared<FactoryDeviceNodeDsp>(),
        VeinTcp::TcpNetworkFactory::create(),
        true);
    qInfo("%s started", qPrintable(zdsp1d->getServerVersion()));

    int r =  app->exec();

    delete zdsp1d;
    delete app;
    return (r);
}
