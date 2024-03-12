#include "mocki2cctrlmmode.h"

MockI2cCtrlMMode::MockI2cCtrlMMode()
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlMMode::setMeasMode(quint8 mmode)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlMMode::readMeasMode(quint8 &mmode)
{
    mmode = 0;
    return ZeraMControllerIo::atmelRM::cmddone;
}
