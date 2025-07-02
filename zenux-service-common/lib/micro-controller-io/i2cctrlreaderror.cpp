#include "i2cctrlreaderror.h"

I2cCtrlReadError::I2cCtrlReadError(QString deviceNodeName, quint8 i2cAddressSysCtrl, quint8 i2cAddressRelCtrl, quint8 debugLevel) :
    m_ctrlIoSysCtrl(deviceNodeName, i2cAddressSysCtrl, debugLevel),
    m_ctrlIoRelCtrl(deviceNodeName, i2cAddressRelCtrl, debugLevel)
{
}


enum hw_cmdcode
{
    hwGetErrorSysCtrl = 0x0204,
    hwGetErrorRelCtrl = 0x02FF,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlReadError::readErrorSysCtrl(QString &errorReply)
{
    return m_ctrlIoSysCtrl.readVariableLenText(hwGetErrorSysCtrl, errorReply);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlReadError::readErrorRelCtrl(QString &errorReply)
{
    return m_ctrlIoRelCtrl.readVariableLenText(hwGetErrorRelCtrl, errorReply);
}

