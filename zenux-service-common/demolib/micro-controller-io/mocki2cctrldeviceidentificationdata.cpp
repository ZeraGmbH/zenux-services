#include "mocki2cctrldeviceidentificationdata.h"

MockI2cCtrlDeviceIdentificationData::MockI2cCtrlDeviceIdentificationData(QString &deviceName,
                                                                     QString &serialNumber,
                                                                     QString &LCAVersion,
                                                                     QString &writablePcbVersion) :
    m_deviceName(deviceName),
    m_serialNumber(serialNumber),
    m_LCAVersion(LCAVersion),
    m_writablePcbVersion(writablePcbVersion)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdentificationData::readDeviceName(QString &answer)
{
    answer = m_deviceName;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdentificationData::readSerialNumber(QString &answer)
{
    answer = m_serialNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdentificationData::writeSerialNumber(QString &sNumber)
{
    m_serialNumber = sNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdentificationData::readLCAVersion(QString &answer)
{
    answer = m_LCAVersion;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdentificationData::writePCBVersion(QString &sVersion)
{
    m_writablePcbVersion = sVersion;
    return ZeraMControllerIo::cmddone;
}
