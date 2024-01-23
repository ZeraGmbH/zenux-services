#include "testi2cctrlaccu.h"

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlAccu::readAccuStatus(quint8 &stat)
{
    Q_UNUSED(stat)
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlAccu::readAccuStateOfCharge(quint8 &charge)
{
    charge = 37;
    return ZeraMControllerIoTemplate::cmddone;
}
