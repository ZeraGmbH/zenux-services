#include "zdspserver.h"
#include "pcbserver.h"
#include "factorydevicenodedsp.h"
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);

    ZDspServer* zdsp1d = new ZDspServer(
        std::make_unique<SettingsContainer>(ZDspServer::defaultParams),
        std::make_shared<FactoryDeviceNodeDsp>());
    qInfo("%s started", qPrintable(zdsp1d->getServerVersion()));

    int r =  app->exec();
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error") ;

    delete zdsp1d;
    return (r);
}
