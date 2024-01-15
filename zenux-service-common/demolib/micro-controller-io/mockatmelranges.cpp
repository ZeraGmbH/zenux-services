#include "mockatmelranges.h"

MockAtmelRanges::MockAtmelRanges() {}

ZeraMControllerIoTemplate::atmelRM MockAtmelRanges::readRange(quint8 channel, quint8 &range)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelRanges::setRange(quint8 channel, quint8 range)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
