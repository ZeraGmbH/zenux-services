#include "ctrlheartbeatwait.h"
#include <QCoreApplication>
#include <QObject>

int main( int argc, char *argv[] )
{
    QCoreApplication app(argc, argv);

    CtrlHeartbeatWait microControllerWait("/dev/zFPGA1reg");

    // Notes:
    // * CtrlHeartbeatWait has enough logging around
    // * If the requirement pops up to return app with other than 0 it needs a
    //   different approach - (remember: exit(int retcode) is not a slot in Qt5!)

    QObject::connect(&microControllerWait, &CtrlHeartbeatWait::sigRunning,
                     &app, QCoreApplication::quit,
                     Qt::QueuedConnection);
    QObject::connect(&microControllerWait, &CtrlHeartbeatWait::sigTimeout,
                     &app, QCoreApplication::quit,
                     Qt::QueuedConnection);

    microControllerWait.start();

    return app.exec();
}
