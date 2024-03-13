#include "demoi2cctrlranges.h"

DemoI2cCtrlRanges::DemoI2cCtrlRanges() {}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlRanges::readRange(quint8 channel, quint8 &range)
{
    range = 0;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlRanges::setRange(quint8 channel, quint8 range)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
