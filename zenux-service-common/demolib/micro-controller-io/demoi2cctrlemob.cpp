#include "emobdefinitions.h"
#include "demoi2cctrlemob.h"
#include "controllerpersitentdata.h"

DemoI2cCtrlEMOB::DemoI2cCtrlEMOB(qint8 muxChannel) :
    m_muxChannel(muxChannel)
{
}

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
    AbstractMockAllServices::hotplugI2cBus device = ControllerPersitentData::getData().m_hotpluggedDevices.value(m_muxChannel);
    const QString ctrlName = device.controllerName;
    if(!ctrlName.isEmpty()) {
        answer = ctrlName + "-DEMO-00V00";
        return ZeraMControllerIo::atmelRM::cmddone;
    }
    return ZeraMControllerIo::atmelRM::cmdexecfault;
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
