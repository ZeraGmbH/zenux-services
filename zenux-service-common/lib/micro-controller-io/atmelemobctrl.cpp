#include "atmelemobctrl.h"
#include "i2cmuxerscopedonoff.h"

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetPCBVersion = 0x0005,
};

AtmelEmobCtrl::AtmelEmobCtrl(QString devnode, quint8 adrCtrl, quint8 adrMux, quint8 muxChannel, quint8 debuglevel) :
    ZeraMcontrollerBase(devnode, adrCtrl, debuglevel)
{
    m_i2cMuxer = I2cMultiplexerFactory::createPCA9547Muxer(devnode, adrMux, muxChannel);
}

ZeraMcontrollerBase::atmelRM AtmelEmobCtrl::readCTRLVersion(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return readVariableLenText(hwGetCtrlVersion, answer);
}

ZeraMcontrollerBase::atmelRM AtmelEmobCtrl::readPCBVersion(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    return readVariableLenText(hwGetPCBVersion, answer);
}
