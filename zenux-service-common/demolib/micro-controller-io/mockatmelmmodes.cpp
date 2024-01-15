#include "mockatmelmmodes.h"

MockAtmelMModes::MockAtmelMModes()
{
}

ZeraMControllerIoTemplate::atmelRM MockAtmelMModes::setMeasMode(quint8 mmode)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelMModes::readMeasMode(quint8 &mmode)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
