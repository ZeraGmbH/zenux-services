#include "sec1000d.h"
#include "factorydevicenodesec.h"
#include <tcpnetworkfactory.h>
#include <QCoreApplication>
#include <QFile>
//#include <QTextCodec>
#include <fcntl.h>
#include <unistd.h>

static int deduceEcUnitCount()
{
    // stolen from cStatusModuleInit::findDeviceType (another one is in ModulemanagerConfig::getDevNameFromUBoot)
    // TODO: Find a common place
    QString zenuxDevice = QStringLiteral("unknown");
    QString procKernelParamFilename = QStringLiteral("/proc/cmdline");
    QFile file(procKernelParamFilename);
    if(file.open(QIODevice::ReadOnly)) {
        const QStringList kernelParams = QString::fromLatin1(file.readAll()).split(QStringLiteral(" "));
        QString paramSearch = QStringLiteral("zera_device=");
        for(auto param : kernelParams) {
            if(param.contains(paramSearch)) {
                zenuxDevice = param.replace(paramSearch, QString()).trimmed();
                break;
            }
        }
        file.close();
    }
    int ecUnitCount = cSEC1000dServer::Mtxxxs2EcUnitCount;
    if (zenuxDevice.toUpper().contains("COM5003"))
        ecUnitCount = cSEC1000dServer::Com5003EcUnitCount;
    qInfo("Available Ec units for %s: %i", qPrintable(zenuxDevice), ecUnitCount);
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
