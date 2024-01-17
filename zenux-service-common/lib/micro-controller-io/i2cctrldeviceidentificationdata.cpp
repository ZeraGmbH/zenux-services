#include "i2cctrldeviceidentificationdata.h"

I2cCtrlDeviceIdentificationData::I2cCtrlDeviceIdentificationData(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwGetSerialNr = 0x0001,
    hwGetDevName = 0x0002,
    hwGetLCAVersion = 0x0004,
    hwSetSerialNr = 0x0006,
    hwSetPCBVersion = 0x0007,

};

ZeraMControllerIoTemplate::atmelRM I2cCtrlDeviceIdentificationData::readDeviceName(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetDevName, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlDeviceIdentificationData::readSerialNumber(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetSerialNr, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlDeviceIdentificationData::writeSerialNumber(QString &sNumber)
{
    ZeraMControllerIo::atmelRM ret;
    quint16 len = static_cast<quint16>(sNumber.length());
    if ( (len<1) || (len>24)) {
        ret = ZeraMControllerIo::cmdfault;
    }
    else {
        QByteArray ba = sNumber.toLatin1();
        hw_cmd CMD(hwSetSerialNr, 0, reinterpret_cast<quint8*>(ba.data()), len);
        m_ctrlIo.writeCommand(&CMD);
        ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    }
    return ret;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlDeviceIdentificationData::readLCAVersion(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetLCAVersion, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlDeviceIdentificationData::writePCBVersion(QString &sVersion)
{
    ZeraMControllerIo::atmelRM ret;
    quint16 len = static_cast<quint16>(sVersion.length());
    if (len<1 || len>24) {
        ret = ZeraMControllerIo::cmdfault;
    }
    else {
        QByteArray ba = sVersion.toLatin1();
        hw_cmd CMD(hwSetPCBVersion, 0, reinterpret_cast<quint8*>(ba.data()), len);
        m_ctrlIo.writeCommand(&CMD);
        ret = m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
    }
    return ret;
}
