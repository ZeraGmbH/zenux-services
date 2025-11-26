#include "testi2cctrlemob.h"
#include "controllerpersitentdata.h"
#include "emobdefinitions.h"

TestI2cCtrlEMOB::TestI2cCtrlEMOB(qint8 muxChannel) :
    m_muxChannel(muxChannel)
{
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::sendPushbuttonPress()
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel))
        return ZeraMControllerIo::atmelRM::cmddone;
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::readEmobLockState(quint8 &status)
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel)) {
        status = reademoblockstate::emobstate_open;
        return ZeraMControllerIo::atmelRM::cmddone;
    }
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::readEmobInstrumentSubType(QString &answer)
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel)) {
        answer = ControllerPersitentData::getData().m_hotpluggedDevices[m_muxChannel].controllerName;
        return ZeraMControllerIo::atmelRM::cmddone;
    }
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}


ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::readEmobErrorStatus(quint8 &err)
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel)) {
        err = errorInstrumentStatus::Instrument_Status_Cable_Error;
        return ZeraMControllerIo::atmelRM::cmddone;
    }
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlEMOB::clearErrorStatus()
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel))
        return ZeraMControllerIo::atmelRM::cmddone;
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}
