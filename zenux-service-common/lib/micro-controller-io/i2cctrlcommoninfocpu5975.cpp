#include "i2cctrlcommoninfocpu5975.h"
#include "i2cmuxerscopedonoff.h"

I2cCtrlCommonInfoCpu5975::I2cCtrlCommonInfoCpu5975(QString deviceNodeName,
                                             quint8 i2cAddress,
                                             quint8 i2cAddressMux,
                                             qint8 muxChannel,
                                             quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel),
    m_i2cMuxer(I2cMultiplexerFactory::createPCA9547Muxer(deviceNodeName, i2cAddressMux, muxChannel))
{
}

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetPCBVersion = 0x0005,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonInfoCpu5975::readPCBInfo(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return m_ctrlIo.readVariableLenText(hwGetPCBVersion, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonInfoCpu5975::readCTRLVersion(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return m_ctrlIo.readVariableLenText(hwGetCtrlVersion, answer);
}
