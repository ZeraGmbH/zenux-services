// test programm scpi parser

#include <iostream>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <QCoreApplication>
#include <QTextCodec>
#include "sec1000dglobal.h"
#include "sec1000d.h"



int main( int argc, char *argv[] )
{

    openlog(ServerName, LOG_PID, LOG_DAEMON); // open connection to syslogd

    QCoreApplication* app = new QCoreApplication (argc, argv);
    cSEC1000dServer* sec5003d=new cSEC1000dServer(); // this is our server

    qInfo(ServerName " started");

    int r = app->exec();
    if (r == rmConnectionError)
        syslog(LOG_EMERG,"Abort, resourcemanager connection error\n") ;

    syslog(LOG_INFO,"sec1000d server child process terminated ret = %d\n", r);
    delete sec5003d;
    closelog();
    return (r);
}
