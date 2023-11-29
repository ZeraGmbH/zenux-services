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
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error") ;

    delete zdsp1d;
    return (r);
}
