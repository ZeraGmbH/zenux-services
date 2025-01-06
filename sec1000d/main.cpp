#include <fcntl.h>
#include <unistd.h>
#include <QCoreApplication>
#include <QTextCodec>
#include "factorydevicenodesec.h"
#include <tcpnetworkfactory.h>
#include "sec1000d.h"

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);

    cSEC1000dServer* sec5003d = new cSEC1000dServer(
        std::make_unique<SettingsContainer>(cSEC1000dServer::defaultParams),
        std::make_shared<FactoryDeviceNodeSec>(),
        VeinTcp::TcpNetworkFactory::create());
    qInfo("%s started", qPrintable(sec5003d->getServerVersion()));

    int r = app->exec();

    delete sec5003d;
    delete app;
    return (r);
}
