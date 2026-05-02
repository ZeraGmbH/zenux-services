#include "asyncqueuedmcontrollerio.h"
#include "asynczeramcontrollertask.h"

void AsyncQueuedMControllerIo::startAsyncRangeIo(AbstractZeraMControllerDelayedPtr mcontroller,
                                                 const ProtonetCommandPtr &protoCmd,
                                                 const QString &rangeName)
{
    std::unique_ptr<AsyncZeraMControllerTask> task = AsyncZeraMControllerTask::create(
        std::move(mcontroller),
        protoCmd,
        rangeName);
    connect(task.get(), &AsyncZeraMControllerTask::sigCmdDone, this, [this](const ProtonetCommandPtr &protoCmd, QString rangeName) {
        emit sigCmdDone(protoCmd, rangeName);
    });
    m_pendingTaskQueue.addSub(std::move(task));
}
