// test programm scpi parser

#include <iostream>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <QCoreApplication>
#include <QTextCodec>
#include "com5003dglobal.h"
#include "com5003d.h"



int main( int argc, char *argv[] )
{

    openlog(ServerName, LOG_PID, LOG_DAEMON); // open connection to syslogd

    QCoreApplication* app = new QCoreApplication (argc, argv);
    cCOM5003dServer* com5003d=new cCOM5003dServer(); // this is our server

    qInfo(ServerName " started");

    int r = app->exec();
    if (r == xsdfileError)
        qCritical("Abort, xsd file error\n") ;
    if (r == xmlfileError)
        qCritical("Abort, xml file error\n") ;
    if (r == atmelError)
        qCritical("Abort, atmel not running\n") ;
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error\n") ;
    if (r == atmelProgError)
        qCritical("Abort, atmel flash program error\n") ;

    syslog(LOG_INFO,"com5003d server child process terminated ret = %d\n", r);
    delete com5003d;
    closelog();
    return (r);
}
