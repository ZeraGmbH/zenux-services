#include "mocki2cctrlemob.h"
#include "controllerpersitentdata.h"
#include "emobdefinitions.h"

MockI2cCtrlEMOB::MockI2cCtrlEMOB(qint8 muxChannel, const QString &instrumentTypePostfix) :
    m_muxChannel(muxChannel),
    m_instrumentTypePostfix(instrumentTypePostfix)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::sendPushbuttonPress()
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel))
        return ZeraMControllerIo::atmelRM::cmddone;
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readEmobLockState(quint8 &status)
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel)) {
        status = reademoblockstate::emobstate_open;
        return ZeraMControllerIo::atmelRM::cmddone;
    }
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readEmobInstrumentSubType(QString &answer)
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel)) {
        answer = ControllerPersitentData::getData().m_hotpluggedDevices[m_muxChannel].controllerName +
                 m_instrumentTypePostfix;
        return ZeraMControllerIo::atmelRM::cmddone;
    }
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}


ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readEmobErrorStatus(quint8 &err)
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel)) {
        err = errorInstrumentStatus::Instrument_Status_Cable_Error;
        return ZeraMControllerIo::atmelRM::cmddone;
    }
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::clearErrorStatus()
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel))
        return ZeraMControllerIo::atmelRM::cmddone;
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::readExchangeData(QByteArray &answer)
{
    answer = getDefaultExchangeData();
    return ZeraMControllerIo::atmelRM::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlEMOB::writeExchangeData(QByteArray &data, int senderId)
{
    if (ControllerPersitentData::isHotControllerAvailable(m_muxChannel)) {
        ControllerPersitentData::getData().m_hotpluggedDevices[m_muxChannel].emobDataReceived[senderId] = data;
        return ZeraMControllerIo::atmelRM::cmddone;
    }
    return ZeraMControllerIo::atmelRM::cmdexecfault;
}

QByteArray MockI2cCtrlEMOB::getDefaultExchangeData()
{
    QByteArray data;
    data.resize(256);
    for (int i=data.size()-1; i>=0; --i)
        data[i] = i;
    return data;
}

