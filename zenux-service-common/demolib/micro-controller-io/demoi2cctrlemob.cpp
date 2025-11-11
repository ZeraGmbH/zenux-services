#include "emobdefinitions.h"
#include "demoi2cctrlemob.h"

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlEMOB::sendPushbuttonPress()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlEMOB::readEmobLockState(quint8 &status)
{
    status = reademoblockstate::emobstate_open;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlEMOB::readEmobInstrumentSubType(QString &answer)
{
    answer = "EMOB_MOCK-00V00";
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlEMOB::readEmobErrorStatus(quint8 &err)
{
    err = errorInstrumentStatus::Instrument_Status_Cable_Error;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlEMOB::clearErrorStatus()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
