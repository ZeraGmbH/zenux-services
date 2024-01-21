#include "mt310s2d.h"
#include "mt310s2dglobal.h"
#include "factoryi2cctrl.h"
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);

    std::shared_ptr<SettingsForDeviceServer> settings = std::make_shared<SettingsForDeviceServer>(cMT310S2dServer::defaultParams);
    std::shared_ptr<FactoryI2cCtrl> ctrlFactory = std::make_shared<FactoryI2cCtrl>(settings->getI2cSettings());
    cMT310S2dServer* mt310s2d = new cMT310S2dServer(std::move(settings), ctrlFactory); // this is our server
    qInfo(ServerName " started");

    int r = app->exec();
    if (r == atmelError)
        qCritical("Abort, atmel not running") ;
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error") ;

    delete mt310s2d;
    return (r);
}
