#include "i2cctrlcommoninfo.h"

I2cCtrlCommonInfo::I2cCtrlCommonInfo(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetPCBVersion = 0x0005,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonInfo::readPCBInfo(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetPCBVersion, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonInfo::readCTRLVersion(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetCtrlVersion, answer);
}
