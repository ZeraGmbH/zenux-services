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

    int r;

#if !defined(COM5003DDEBUG) && !defined(SYSTEMD_NOTIFICATION)

    pid_t pid;
    if ( (pid=fork() ) < 0 ) // we generate a child process
    {
        syslog(LOG_EMERG,"fork() failed\n") ; // error message to syslogd if not
        return (1);
    }

    if (pid==0) // if we have a child process now
    {
        syslog(LOG_INFO,"sec1000d server child process created\n");
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
#endif // SEC1000DDEBUG

    if (r == parameterError)
        syslog(LOG_EMERG,"Abort, wrong parameter count\n") ;
    if (r == pipeError)
        syslog(LOG_EMERG,"Abort, could not open pipe\n") ;
    if (r == xsdfileError)
        syslog(LOG_EMERG,"Abort, xsd file error\n") ;
    if (r == xmlfileError)
        syslog(LOG_EMERG,"Abort, xml file error\n") ;
    if (r == secDeviceError)
        syslog(LOG_EMERG,"Abort, sec device not found\n") ;
    if (r == rmConnectionError)
        syslog(LOG_EMERG,"Abort, resourcemanager connection error\n") ;

    syslog(LOG_INFO,"sec1000d server child process terminated ret = %d\n", r);
    delete sec5003d;
    closelog();
    return (r);
}
