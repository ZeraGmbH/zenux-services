#include "mockatmelaccumulatorhandler.h"

// Currently there is unmocked testmode. Once we ged rid of this, we have to
// add more in here

ZeraMControllerIoTemplate::atmelRM MockAtmelAccumulatorHandler::readAccumulatorStatus(quint8 &stat)
{
    Q_UNUSED(stat)
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelAccumulatorHandler::readAccuStateOfCharge(quint8 &charge)
{
    Q_UNUSED(charge)
    return ZeraMControllerIoTemplate::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelAccumulatorHandler::enableTestMode(qint32 testBits)
{
    Q_UNUSED(testBits)
    return ZeraMControllerIoTemplate::cmddone;
}
