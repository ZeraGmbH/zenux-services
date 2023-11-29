#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <QCoreApplication>
#include <QTextCodec>
#include "sec1000d.h"

int main( int argc, char *argv[] )
{
    QCoreApplication* app = new QCoreApplication (argc, argv);
    cSEC1000dServer* sec5003d = new cSEC1000dServer(); // this is our server
    qInfo("%s started", qPrintable(sec5003d->getServerVersion()));

    int r = app->exec();
    if (r == rmConnectionError)
        qCritical("Abort, resourcemanager connection error");

    delete sec5003d;
    return (r);
}
