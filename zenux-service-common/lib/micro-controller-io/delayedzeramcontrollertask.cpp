#include "delayedzeramcontrollertask.h"
#include <zscpi_response_definitions.h>

std::unique_ptr<DelayedZeraMControllerTask> DelayedZeraMControllerTask::create(
    AbstractZeraMControllerDelayedPtr mcontroller,
    ProtonetCommandPtr protoCmd,
    const QString &rangeName)
{
    return std::make_unique<DelayedZeraMControllerTask>(std::move(mcontroller),
                                                        protoCmd,
                                                        rangeName);
}

DelayedZeraMControllerTask::DelayedZeraMControllerTask(AbstractZeraMControllerDelayedPtr mcontroller,
                                                       ProtonetCommandPtr protoCmd,
                                                       const QString &rangeName) :
    m_mcontroller(std::move(mcontroller)),
    m_protoCmd(protoCmd),
    m_rangeName(rangeName)
{
    connect(m_mcontroller.get(), &AbstractZeraMControllerDelayed::sigCmdDone,
            this, &DelayedZeraMControllerTask::onCmdDone);
}

void DelayedZeraMControllerTask::start()
{
    // controller was started and MUST operate unblocked
}

void DelayedZeraMControllerTask::onCmdDone(ZeraMControllerIoTemplate::atmelRM cmdResult)
{
    bool cmdOk = cmdResult == ZeraMControllerIoTemplate::cmddone;
    m_protoCmd->m_sOutput = ZSCPI::scpiAnswer[cmdOk ? ZSCPI::ack : ZSCPI::errexec];
    emit sigCmdDone(m_protoCmd, m_rangeName);
    finishTask(true); // error handling is not the scope in here
}
