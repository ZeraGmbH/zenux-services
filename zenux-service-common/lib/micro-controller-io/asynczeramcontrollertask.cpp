#include "asynczeramcontrollertask.h"
#include <zscpi_response_definitions.h>

std::unique_ptr<AsyncZeraMControllerTask> AsyncZeraMControllerTask::create(
    AbstractZeraMControllerDelayedPtr mcontroller,
    ProtonetCommandPtr protoCmd,
    const QString &rangeName)
{
    return std::make_unique<AsyncZeraMControllerTask>(std::move(mcontroller),
                                                        protoCmd,
                                                        rangeName);
}

AsyncZeraMControllerTask::AsyncZeraMControllerTask(AbstractZeraMControllerDelayedPtr mcontroller,
                                                       ProtonetCommandPtr protoCmd,
                                                       const QString &rangeName) :
    m_mcontroller(std::move(mcontroller)),
    m_protoCmd(protoCmd),
    m_rangeName(rangeName)
{
    connect(m_mcontroller.get(), &AbstractZeraMControllerDelayed::sigCmdDone,
            this, &AsyncZeraMControllerTask::onCmdDone);
}

void AsyncZeraMControllerTask::start()
{
    m_mcontroller->startCmdIfNotStarted();
}

void AsyncZeraMControllerTask::onCmdDone(ZeraMControllerIoTemplate::atmelRM cmdResult)
{
    bool cmdOk = cmdResult == ZeraMControllerIoTemplate::cmddone;
    m_protoCmd->m_sOutput = ZSCPI::scpiAnswer[cmdOk ? ZSCPI::ack : ZSCPI::errexec];
    emit sigCmdDone(m_protoCmd, m_rangeName);
    finishTask(true); // error handling is not the scope in here
}
