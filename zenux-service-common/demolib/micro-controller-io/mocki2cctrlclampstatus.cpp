#include "mocki2cctrlclampstatus.h"

MockI2cCtrlClampStatus::MockI2cCtrlClampStatus()
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlClampStatus::readClampStatus(quint16 &stat)
{
    stat = 0;
    return ZeraMControllerIo::atmelRM::cmddone;
}
