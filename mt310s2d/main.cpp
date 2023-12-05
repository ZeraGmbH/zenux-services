#include "mt310s2d.h"
#include "mt310s2dglobal.h"
#include <QCoreApplication>
#include <syslog.h>

int main( int argc, char *argv[] )
{
    openlog(ServerName, LOG_PID, LOG_DAEMON); // we are still using it

    QCoreApplication* app = new QCoreApplication (argc, argv);
    cMT310S2dServer* mt310s2d = new cMT310S2dServer(); // this is our server
    qInfo(ServerName " started");

    int r = app->exec();
    if (r == atmelError)
        qCritical("Abort, atmel not running") ;
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error") ;

    syslog(LOG_INFO,"mt310s2d server child process terminated ret = %d\n", r);
    delete mt310s2d;
    closelog();
    return (r);
}
