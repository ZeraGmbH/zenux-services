#include "com5003d.h"
#include "com5003dglobal.h"
#include "factorydevicenodepcb.h"
#include "factoryi2cctrl.h"
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);

    std::unique_ptr<SettingsContainer> settings = std::make_unique<SettingsContainer>(cCOM5003dServer::defaultParams);
    std::shared_ptr<FactoryI2cCtrl> ctrlFactory = std::make_shared<FactoryI2cCtrl>(settings->getI2cSettings());
    cCOM5003dServer* com5003d = new cCOM5003dServer(
        std::move(settings),
        ctrlFactory,
        std::make_shared<FactoryDeviceNodePcb>());
    qInfo(ServerName " started");

    int r = app->exec();
    if (r == atmelError)
        qCritical("Abort, atmel not running") ;
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error") ;
    if (r == atmelProgError)
        qCritical("Abort, atmel flash program error") ;

    delete com5003d;
    return (r);
}
