#include "coredumpwatcher.h"

CoreDumpWatcher::CoreDumpWatcher(QString coredumpDir, QList<int> userIdsToWatch)
{
    QMetaObject::invokeMethod(this,
                              "sigCoredumpCreated",
                              Qt::QueuedConnection);
}
