#include "i2cctrlcommoninfoemob.h"
#include "i2cmuxerscopedonoff.h"

I2cCtrlCommonInfoEmob::I2cCtrlCommonInfoEmob(QString deviceNodeName,
                                             quint8 i2cAddress,
                                             quint8 i2cAddressMux,
                                             quint8 muxChannel,
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

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonInfoEmob::readPCBInfo(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return m_ctrlIo.readVariableLenText(hwGetPCBVersion, answer);
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlCommonInfoEmob::readCTRLVersion(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return m_ctrlIo.readVariableLenText(hwGetCtrlVersion, answer);
}
