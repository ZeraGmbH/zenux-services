#include "demoi2cctrldeviceident.h"

DemoI2cCtrlDeviceIdent::DemoI2cCtrlDeviceIdent(QString &serialNumber,
                                               QString &LCAVersion,
                                               QString &writablePcbVersion) :
    m_serialNumber(serialNumber),
    m_FPGAVersion(LCAVersion),
    m_writablePcbVersion(writablePcbVersion)
{
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlDeviceIdent::readDeviceName(QString &answer)
{
    answer = "Demo device name";
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlDeviceIdent::readSerialNumber(QString &answer)
{
    answer = m_serialNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlDeviceIdent::writeSerialNumber(QString &sNumber)
{
    m_serialNumber = sNumber;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlDeviceIdent::readLCAVersion(QString &answer)
{
    answer = m_FPGAVersion;
    return ZeraMControllerIo::cmddone;
}

ZeraMControllerIoTemplate::atmelRM DemoI2cCtrlDeviceIdent::writePCBVersion(QString &sVersion)
{
    m_writablePcbVersion = sVersion;
    return ZeraMControllerIo::cmddone;
}
