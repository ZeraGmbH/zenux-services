#include "testi2cctrlemob.h"
#include "controllerpersitentdata.h"
#include "emobdefinitions.h"

TestI2cCtrlEMOB::TestI2cCtrlEMOB(int ctrlMuxChannnel) :
    m_ctrlMuxChannnel(ctrlMuxChannnel)
{
}

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
    if (ControllerPersitentData::getData().m_hotpluggedDevices.contains(m_ctrlMuxChannnel))
        answer = ControllerPersitentData::getData().m_hotpluggedDevices[m_ctrlMuxChannnel];
    return ZeraMControllerIo::atmelRM::cmddone;
}


ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::readEmobErrorStatus(quint8 &err)
{
    err = errorInstrumentStatus::Instrument_Status_Cable_Error;
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::clearErrorStatus()
{
    return ZeraMControllerIo::atmelRM::cmddone;
}
