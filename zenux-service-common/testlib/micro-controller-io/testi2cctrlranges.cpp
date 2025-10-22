#include "testi2cctrlranges.h"
#include <timerfactoryqt.h>

TestI2cCtrlRanges::TestI2cCtrlRanges(int delayedResponse) :
    m_delayedResponse(delayedResponse),
    m_delayTimer(TimerFactoryQt::createPeriodic(delayedResponse))
{
    if(delayedResponse != 0)
        connect(m_delayTimer.get(), &TimerTemplateQt::sigExpired,
                this, [this]() {
            emit sigCmdDone(ZeraMControllerIo::atmelRM::cmddone);
        });
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlRanges::readRange(quint8 channel, quint8 &range)
{
    Q_UNUSED(channel)
    range = 0;
    if (m_delayedResponse == 0)
        return ZeraMControllerIo::atmelRM::cmddone;

    m_delayTimer->start();
    return ZeraMControllerIo::atmelRM::cmdpending;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlRanges::setRange(quint8 channel, quint8 range)
{
    Q_UNUSED(channel)
    Q_UNUSED(range)
    if (m_delayedResponse == 0)
        return ZeraMControllerIo::atmelRM::cmddone;

    m_delayTimer->start();
    return ZeraMControllerIo::atmelRM::cmdpending;
}
