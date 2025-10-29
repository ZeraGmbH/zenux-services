#ifndef ASYNCQUEUEDMCONTROLLERIO_H
#define ASYNCQUEUEDMCONTROLLERIO_H

#include "abstractzeramcontrollerdelayed.h"
#include "protonetcommand.h"
#include <taskcontainersequence.h>

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
    TaskContainerSequence m_pendingTaskContainer;
};

#endif // ASYNCQUEUEDMCONTROLLERIO_H
