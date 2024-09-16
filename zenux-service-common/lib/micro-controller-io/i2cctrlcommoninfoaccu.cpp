#include "i2cctrlcommoninfoaccu.h"

I2cCtrlCommonInfoAccu::I2cCtrlCommonInfoAccu(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

constexpr int tunnelParamLen = 3;
constexpr quint8 accuI2cAddress = 0x0A;
constexpr quint8 accuCmdIdLogicalDeviceNo = 0x00;

constexpr quint8 cmdIdTunnelI2c = 0x0054;
constexpr quint8 cmdAccuGetCtrlVersion = 0x0003;
constexpr quint8 cmdAccuGetPCBVersion = 0x0005;

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonInfoAccu::readPCBInfo(QString &answer)
{
    quint8 tunnelParam[tunnelParamLen];
    tunnelParam[0] = accuI2cAddress;
    tunnelParam[1] = accuCmdIdLogicalDeviceNo;
    tunnelParam[2] = cmdAccuGetPCBVersion;
    return m_ctrlIo.readVariableLenText(cmdIdTunnelI2c, answer, tunnelParam, tunnelParamLen);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonInfoAccu::readCTRLVersion(QString &answer)
{
    quint8 tunnelParam[tunnelParamLen];
    tunnelParam[0] = accuI2cAddress;
    tunnelParam[1] = accuCmdIdLogicalDeviceNo;
    tunnelParam[2] = cmdAccuGetCtrlVersion;
    return m_ctrlIo.readVariableLenText(cmdIdTunnelI2c, answer, tunnelParam, tunnelParamLen);
}
