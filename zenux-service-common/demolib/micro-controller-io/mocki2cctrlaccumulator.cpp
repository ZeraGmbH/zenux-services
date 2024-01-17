#include "mocki2cctrlaccumulator.h"

// Currently there is unmocked testmode. Once we ged rid of this, we have to
// add more in here

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlAccumulator::readAccumulatorStatus(quint8 &stat)
{
    Q_UNUSED(stat)
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlAccumulator::readAccuStateOfCharge(quint8 &charge)
{
    charge = 37;
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlAccumulator::enableTestMode(qint32 testBits)
{
    Q_UNUSED(testBits)
    return ZeraMControllerIoTemplate::cmddone;
}
