#include "asyncqueuedmcontrollerio.h"
#include "delayedzeramcontrollertask.h"

void AsyncQueuedMControllerIo::startAsyncRangeIo(AbstractZeraMControllerDelayedPtr mcontroller,
                                                            ProtonetCommandPtr protoCmd,
                                                            const QString &rangeName)
{
    std::unique_ptr<DelayedZeraMControllerTask> task = DelayedZeraMControllerTask::create(
        std::move(mcontroller),
        protoCmd,
        rangeName);
    connect(task.get(), &DelayedZeraMControllerTask::sigCmdDone, this, [this](ProtonetCommandPtr protoCmd, QString rangeName) {
        emit sigCmdDone(protoCmd, rangeName);
    });
    m_pendingTaskContainer.addSub(std::move(task));
    m_pendingTaskContainer.start();
}
