#include "i2cctrlcommonversions.h"

I2cCtrlCommonVersions::I2cCtrlCommonVersions(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetPCBVersion = 0x0005,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonVersions::readPCBVersion(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetPCBVersion, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonVersions::readCTRLVersion(QString &answer)
{
    return m_ctrlIo.readVariableLenText(hwGetCtrlVersion, answer);
}
