#include "com5003d.h"
#include "com5003dglobal.h"
#include "factoryi2cctrl.h"
#include <QCoreApplication>
#include <syslog.h>

int main( int argc, char *argv[] )
{
    openlog(ServerName, LOG_PID, LOG_DAEMON); // we are still using it...

    QCoreApplication* app = new QCoreApplication (argc, argv);

    std::shared_ptr<SettingsForDeviceServer> settings = std::make_shared<SettingsForDeviceServer>(cCOM5003dServer::defaultParams);
    std::shared_ptr<FactoryI2cCtrl> ctrlFactory = std::make_shared<FactoryI2cCtrl>(settings->getI2cSettings());
    cCOM5003dServer* com5003d = new cCOM5003dServer(std::move(settings), ctrlFactory); // this is our server
    qInfo(ServerName " started");

    int r = app->exec();
    if (r == atmelError)
        qCritical("Abort, atmel not running") ;
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error") ;
    if (r == atmelProgError)
        qCritical("Abort, atmel flash program error") ;

    delete com5003d;
    closelog();
    return (r);
}
