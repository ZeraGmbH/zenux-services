#include "demoi2cctrlranges.h"
#include "simulsystemstatus.h"

DemoI2cCtrlRanges::DemoI2cCtrlRanges()
{
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlRanges::readRange(quint8 channel, quint8 &range)
{
    range = SimulSystemStatus::getInstance()->getRange(channel);
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlRanges::setRange(quint8 channel, quint8 range)
{
    SimulSystemStatus::getInstance()->setRange(channel, range);
    return ZeraMControllerIo::atmelRM::cmddone;
}
