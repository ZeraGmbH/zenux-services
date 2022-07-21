#include <iostream>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <QCoreApplication>
#include <qstring.h>

#include "zdsp1d.h"
#include "zdspglobal.h"

int main( int argc, char *argv[] )
{
    openlog(ServerBasisName, LOG_PID, LOG_DAEMON); // verbindung zum syslogd aufnehmen

    QCoreApplication* app = new QCoreApplication (argc, argv);
    cZDSP1Server* zdsp1d = new cZDSP1Server(); // this is our server

    int r = 0;
#if !defined(ZDSP1DDEBUG) && !defined(SYSTEMD_NOTIFICATION)
    pid_t pid;
    if ( (pid=fork() ) < 0 ) // we generate a child process
    {
        syslog(LOG_EMERG,"fork() failed\n") ; // error message to syslogd if not
        return (forkError);
    }

    if (pid==0) // if we have a child process now
    {
        syslog(LOG_INFO,"zdsp1d server child process created\n");
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
        r = app->exec(); // id ZDSP1DDEBUG -> no fork -> server runs in foreground
#endif // ZDSP1DDEBUG

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
