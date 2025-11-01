#ifndef ASYNCQUEUEDMCONTROLLERIO_H
#define ASYNCQUEUEDMCONTROLLERIO_H

#include "abstractzeramcontrollerdelayed.h"
#include "protonetcommand.h"
#include <taskcontainerqueue.h>

class AsyncQueuedMControllerIo : public QObject
{
    Q_OBJECT
public:
    ~AsyncQueuedMControllerIo() = default;
    void startAsyncRangeIo(AbstractZeraMControllerDelayedPtr mcontroller,
                      ProtonetCommandPtr protoCmd,
                      const QString &rangeName);
signals:
    void sigCmdDone(ProtonetCommandPtr protoCmd, QString rangeName);
private:
    TaskContainerQueue m_pendingTaskQueue;
};

#endif // ASYNCQUEUEDMCONTROLLERIO_H
