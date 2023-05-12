#include "atmelemobctrl.h"
#include "i2cmuxerscopedonoff.h"

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetPCBVersion = 0x0005,
};

AtmelEmobCtrl::AtmelEmobCtrl(QString devnode, quint8 adr, quint8 ctrlChannelForMux, quint8 debuglevel) :
    ZeraMcontrollerBase(devnode, adr, debuglevel),
    m_i2cMuxer(I2cMultiplexerFactory::createClampMuxer(devnode, adr, ctrlChannelForMux))
{
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
