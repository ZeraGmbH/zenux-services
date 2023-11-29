#include "zdspserver.h"
#include "zdspglobal.h"
#include "pcbserver.h"
#include <iostream>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    openlog(ServerName, LOG_PID, LOG_DAEMON); // verbindung zum syslogd aufnehmen

    QCoreApplication* app = new QCoreApplication (argc, argv);
    ZDspServer* zdsp1d = new ZDspServer(); // this is our server

    qInfo(ServerName " started");

    int r =  app->exec();
    if (r == parameterError)
        syslog(LOG_EMERG,"Abort, wrong parameter count\n") ;
    if (r == pipeError)
        syslog(LOG_EMERG,"Abort, could not open pipe\n") ;
    if (r == xsdfileError)
        syslog(LOG_EMERG,"Abort, xsd file error\n") ;
    if (r == xmlfileError)
        syslog(LOG_EMERG,"Abort, xml file error\n") ;
    if (r == dspDeviceError)
        syslog(LOG_EMERG,"Abort, dsp device not found\n") ;
    if (r == dspBootError)
        syslog(LOG_EMERG,"Abort, dsp device boot error\n") ;
    if (r == dspBootFileError)
        syslog(LOG_EMERG,"Abort, dsp device and bootfile mismatch\n") ;
    if (r == dspSetSamplingError)
        syslog(LOG_EMERG,"Abort, dsp device set sampling system error\n") ;
    if (r == rmConnectionError)
        syslog(LOG_EMERG,"Abort, resourcemanager connection error\n") ;

    syslog(LOG_INFO,"zdsp1d server child process terminated ret = %d\n", r);
    delete zdsp1d;
    closelog();
    return (r);
}
