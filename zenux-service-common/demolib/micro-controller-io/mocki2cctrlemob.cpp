#include "mocki2cctrlemob.h"

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::sendPushbuttonPress()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readEmobLockState(quint8 &status)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
