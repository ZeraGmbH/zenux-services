#include "testi2cctrlemob.h"
#include "emobdefinitions.h"

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::sendPushbuttonPress()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::readEmobLockState(quint8 &status)
{
    status = reademoblockstate::emobstate_open;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::readEmobInstrumentSubType(QString &answer)
{
    answer = "EMOB_MOCK-00V00";
    return ZeraMControllerIo::atmelRM::cmddone;
}
