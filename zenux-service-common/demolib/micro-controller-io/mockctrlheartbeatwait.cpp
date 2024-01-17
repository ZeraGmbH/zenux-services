#include "mockctrlheartbeatwait.h"

void MockCtrlHeartbeatWait::start()
{
    QMetaObject::invokeMethod(this,
                              "sigRunning",
                              Qt::QueuedConnection
                              );
}
