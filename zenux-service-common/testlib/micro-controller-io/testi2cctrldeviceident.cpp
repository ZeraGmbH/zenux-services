#include "testi2cctrldeviceident.h"

TestI2cCtrlDeviceIdent::TestI2cCtrlDeviceIdent(QString &serialNumber,
                                               QString &writablePcbVersion) :
    m_serialNumber(serialNumber),
    m_writablePcbVersion(writablePcbVersion)
{
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlDeviceIdent::readDeviceName(QString &answer)
{
    answer = "Unknown";
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlDeviceIdent::readSerialNumber(QString &answer)
{
    answer = m_serialNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlDeviceIdent::writeSerialNumber(QString &sNumber)
{
    m_serialNumber = sNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM TestI2cCtrlDeviceIdent::readLCAVersion(QString &answer)
{
    answer = "Unknown";
    return ZeraMControllerIo::cmddone;
}
