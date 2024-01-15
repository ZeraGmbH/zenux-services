#include "mockatmeldeviceidentificationdata.h"

MockAtmelDeviceIdentificationData::MockAtmelDeviceIdentificationData(QString &deviceName,
                                                                     QString &serialNumber,
                                                                     QString &LCAVersion,
                                                                     QString &writablePcbVersion) :
    m_deviceName(deviceName),
    m_serialNumber(serialNumber),
    m_LCAVersion(LCAVersion),
    m_writablePcbVersion(writablePcbVersion)
{
}

ZeraMControllerIoTemplate::atmelRM MockAtmelDeviceIdentificationData::readDeviceName(QString &answer)
{
    answer = m_deviceName;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelDeviceIdentificationData::readSerialNumber(QString &answer)
{
    answer = m_serialNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelDeviceIdentificationData::writeSerialNumber(QString &sNumber)
{
    m_serialNumber = sNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelDeviceIdentificationData::readLCAVersion(QString &answer)
{
    answer = m_LCAVersion;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockAtmelDeviceIdentificationData::writePCBVersion(QString &sVersion)
{
    m_writablePcbVersion = sVersion;
    return ZeraMControllerIo::cmddone;
}
