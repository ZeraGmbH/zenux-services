#include "zdspserver.h"
#include "zdspglobal.h"
#include "pcbserver.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);
    ZDspServer* zdsp1d = new ZDspServer(); // this is our server

    qInfo(ServerName " started");

    int r =  app->exec();
    if (r == xsdfileError)
        qCritical("Abort, xsd file error\n") ;
    if (r == xmlfileError)
        qCritical("Abort, xml file error\n") ;
    if (r == dspDeviceError)
        qCritical("Abort, dsp device not found\n") ;
    if (r == dspBootError)
        qCritical("Abort, dsp device boot error\n") ;
    if (r == dspBootFileError)
        qCritical("Abort, dsp device and bootfile mismatch\n") ;
    if (r == dspSetSamplingError)
        qCritical("Abort, dsp device set sampling system error\n") ;
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error\n") ;

    delete zdsp1d;
    return (r);
}
