#include "testi2cctrlranges.h"

TestI2cCtrlRanges::TestI2cCtrlRanges() {}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlRanges::readRange(quint8 channel, quint8 &range)
{
    range = 0;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlRanges::setRange(quint8 channel, quint8 range)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
