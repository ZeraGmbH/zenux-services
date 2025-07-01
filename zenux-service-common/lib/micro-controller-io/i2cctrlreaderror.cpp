#include "i2cctrlreaderror.h"

I2cCtrlReadError::I2cCtrlReadError(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwGetInfo    = 0xFFFF,
    hwGetWarning = 0xFFFF,
    hwGetError   = 0x02FF
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlReadError::readError(QString &errorReply)
{
    return m_ctrlIo.readVariableLenText(hwGetError, errorReply);
}

