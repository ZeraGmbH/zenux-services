#include "mocki2cctrldeviceident.h"

MockI2cCtrlDeviceIdent::MockI2cCtrlDeviceIdent(QString &deviceName,
                                                                     QString &serialNumber,
                                                                     QString &LCAVersion,
                                                                     QString &writablePcbVersion) :
    m_deviceName(deviceName),
    m_serialNumber(serialNumber),
    m_FPGAVersion(LCAVersion),
    m_writablePcbVersion(writablePcbVersion)
{
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdent::readDeviceName(QString &answer)
{
    answer = m_deviceName;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdent::readSerialNumber(QString &answer)
{
    answer = m_serialNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdent::writeSerialNumber(QString &sNumber)
{
    m_serialNumber = sNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdent::readLCAVersion(QString &answer)
{
    answer = m_FPGAVersion;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM MockI2cCtrlDeviceIdent::writePCBVersion(QString &sVersion)
{
    m_writablePcbVersion = sVersion;
    return ZeraMControllerIo::cmddone;
}
