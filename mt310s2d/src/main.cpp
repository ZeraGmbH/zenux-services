// test programm scpi parser

#include <iostream>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <QCoreApplication>
#include <QTextCodec>
#include "mt310s2dglobal.h"
#include "mt310s2d.h"



int main( int argc, char *argv[] )
{

    openlog(ServerName, LOG_PID, LOG_DAEMON); // open connection to syslogd

    QCoreApplication* app = new QCoreApplication (argc, argv);
    cMT310S2dServer* mt310s2d = new cMT310S2dServer(); // this is our server

    qInfo(ServerName " started");

    int r = app->exec();
    if (r == pipeError)
        qCritical( "Abort, could not open pipe\n");
    if (r == xsdfileError)
        qCritical("Abort, xsd file error\n") ;
    if (r == xmlfileError)
        qCritical("Abort, xml file error\n") ;
    if (r == atmelError)
        qCritical("Abort, atmel not running\n") ;
    if (r == ctrlDeviceError)
        qCritical("Abort, ctrl device not found\n") ;
    if (r == fpgaDeviceError)
        qCritical("Abort, fpga device not found\n") ;
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error\n") ;
    if (r == atmelProgError)
        qCritical("Abort, atmel flash program error\n") ;

    syslog(LOG_INFO,"mt310s2d server child process terminated ret = %d\n", r);
    delete mt310s2d;
    closelog();
    return (r);
}
