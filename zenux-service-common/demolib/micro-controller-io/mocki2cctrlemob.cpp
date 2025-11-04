#include "mocki2cctrlemob.h"

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::sendPushbuttonPress()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readEmobLockState(quint8 &status)
{
    Q_UNUSED(status) // for now
    return ZeraMControllerIo::atmelRM::cmddone;
}
