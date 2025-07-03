#include "i2cctrlreaderror.h"

I2cCtrlReadError::I2cCtrlReadError(QString deviceNodeName, quint8 i2cAddressSystemCtrl, quint8 i2cAddressRelayCtrl, quint8 i2cAddressEmobCtrl, quint8 debugLevel) :
    m_ctrlIoSystemCtrl(deviceNodeName, i2cAddressSystemCtrl, debugLevel),
    m_ctrlIoRelayCtrl(deviceNodeName, i2cAddressRelayCtrl, debugLevel),
    m_ctrlIoEmobCtrl(deviceNodeName, i2cAddressEmobCtrl, debugLevel)
{
}


enum hw_cmdcode
{
    hwGetErrorSystemCtrl = 0x0204,
    hwGetErrorRelayCtrl  = 0x02FF,
    hwGetErrorEmoblCtrl  = 0x02FF,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlReadError::readErrorSystemCtrl(QString &errorReply)
{
    return m_ctrlIoSystemCtrl.readVariableLenText(hwGetErrorSystemCtrl, errorReply);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlReadError::readErrorRelayCtrl(QString &errorReply)
{
    return m_ctrlIoRelayCtrl.readVariableLenText(hwGetErrorRelayCtrl, errorReply);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlReadError::readErrorEmobCtrl(QString &errorReply)
{
    return m_ctrlIoEmobCtrl.readVariableLenText(hwGetErrorEmoblCtrl, errorReply);
}

