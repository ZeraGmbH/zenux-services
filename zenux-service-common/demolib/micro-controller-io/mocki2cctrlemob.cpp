#include "mocki2cctrlemob.h"

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::sendPushbuttonPress(QString channelName)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readEmobLockState(quint8 &status, QString channelName)
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
