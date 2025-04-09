#include "sec1000d.h"
#include "factorydevicenodesec.h"
#include "zenuxdevicedetection.h"
#include <tcpnetworkfactory.h>
#include <QCoreApplication>

static int deduceEcUnitCount()
{
    const QString zenuxDevice = ZenuxDeviceDetection::getDeviceNameFromKernelParam();
    int ecUnitCount = cSEC1000dServer::Mtxxxs2EcUnitCount;
    if (zenuxDevice.toUpper().contains("COM5003"))
        ecUnitCount = cSEC1000dServer::Com5003EcUnitCount;
    qInfo("Available error calculator units for %s: %i", qPrintable(zenuxDevice), ecUnitCount);
    return ecUnitCount;
}

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);

    cSEC1000dServer* sec5003d = new cSEC1000dServer(
        std::make_unique<SettingsContainer>(cSEC1000dServer::defaultParams),
        deduceEcUnitCount(),
        std::make_shared<FactoryDeviceNodeSec>(),
        VeinTcp::TcpNetworkFactory::create());
    qInfo("%s started", qPrintable(sec5003d->getServerVersion()));

    int r = app->exec();

    delete sec5003d;
    delete app;
    return (r);
}
