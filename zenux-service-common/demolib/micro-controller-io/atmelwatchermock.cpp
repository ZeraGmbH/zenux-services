#include "atmelwatchermock.h"

void AtmelWatcherMock::start()
{
    QMetaObject::invokeMethod(this,
                              "sigRunning",
                              Qt::QueuedConnection
                              );
}
