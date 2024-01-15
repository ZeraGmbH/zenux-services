#include "mockatmelwatcher.h"

void MockAtmelWatcher::start()
{
    QMetaObject::invokeMethod(this,
                              "sigRunning",
                              Qt::QueuedConnection
                              );
}
