#include "mocki2cctrlranges.h"

MockI2cCtrlRanges::MockI2cCtrlRanges() {}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlRanges::readRange(quint8 channel, quint8 &range)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlRanges::setRange(quint8 channel, quint8 range)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
