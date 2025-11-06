#include "emobdefinitions.h"
#include "mocki2cctrlemob.h"

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::sendPushbuttonPress()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readEmobLockState(quint8 &status)
{
    status = reademoblockstate::emobstate_open;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readEmobInstrumentSubType(QString &answer)
{
    answer = "EMOB_MOCK-00V00";
    return ZeraMControllerIo::atmelRM::cmddone;
}
