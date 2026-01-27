#include "mockctrlheartbeatwait.h"

void MockCtrlHeartbeatWait::start()
{
    // same messages as production
    qInfo("Atmel run-detection started");
    qInfo("Atmel running");
    QMetaObject::invokeMethod(this,
                              "sigRunning",
                              Qt::QueuedConnection
                              );
}
