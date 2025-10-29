#include "asyncqueuedmcontrollerio.h"
#include "asynczeramcontrollertask.h"

void AsyncQueuedMControllerIo::startAsyncRangeIo(AbstractZeraMControllerDelayedPtr mcontroller,
                                                            ProtonetCommandPtr protoCmd,
                                                            const QString &rangeName)
{
    std::unique_ptr<AsyncZeraMControllerTask> task = AsyncZeraMControllerTask::create(
        std::move(mcontroller),
        protoCmd,
        rangeName);
    connect(task.get(), &AsyncZeraMControllerTask::sigCmdDone, this, [this](ProtonetCommandPtr protoCmd, QString rangeName) {
        emit sigCmdDone(protoCmd, rangeName);
    });
    m_pendingTaskContainer.addSub(std::move(task));
    m_pendingTaskContainer.start();
}
