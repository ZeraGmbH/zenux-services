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

    int r;

#if !defined(MT310S2DDEBUG) && !defined(SYSTEMD_NOTIFICATION)
    pid_t pid;
    if ( (pid=fork() ) < 0 ) // we generate a child process
    {
        qCritical("fork() failed\n") ; // error message to syslogd if not
        return (1);
    }

    if (pid==0) // if we have a child process now
    {
        syslog(LOG_INFO,"mt310s2d server child process created\n");
        chdir ("/"); // it becomes a place to live
        setsid();
        close (STDIN_FILENO); // we disconnect from std io and std error o
        close (STDOUT_FILENO);
        close (STDERR_FILENO);
        open ("/dev/null",O_RDWR); // a null device for ev. created output
        dup (STDIN_FILENO);
        dup (STDOUT_FILENO);
        r = app->exec(); // and runs as daemon now

    }
#else
        r = app->exec(); // id DEBUG -> no fork -> server runs in foreground
#endif // MT310S2DDEBUG

    if (r == parameterError)
        qCritical("Abort, wrong parameter count\n") ;
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
