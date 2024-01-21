#include "demoi2cctrlaccu.h"

// Currently there is unmocked testmode. Once we ged rid of this, we have to
// add more in here

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlAccu::readAccuStatus(quint8 &stat)
{
    Q_UNUSED(stat)
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlAccu::readAccuStateOfCharge(quint8 &charge)
{
    charge = 37;
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlAccu::enableTestMode(qint32 testBits)
{
    Q_UNUSED(testBits)
    return ZeraMControllerIoTemplate::cmddone;
}
