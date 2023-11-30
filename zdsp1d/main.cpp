#include "zdspserver.h"
#include "pcbserver.h"
#include "dspdevicenode.h"
#include <QCoreApplication>
#include <memory>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);
    DspDeviceNodeInterfaceUPtr dspDeviceNode = std::make_unique<DspDeviceNode>();
    ZDspServer* zdsp1d = new ZDspServer(std::move(dspDeviceNode)); // this is our server
    qInfo("%s started", qPrintable(zdsp1d->getServerVersion()));

    int r =  app->exec();
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error") ;

    delete zdsp1d;
    return (r);
}
