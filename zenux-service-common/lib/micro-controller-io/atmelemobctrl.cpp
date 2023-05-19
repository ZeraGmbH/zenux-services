#include "atmelemobctrl.h"
#include "i2cmuxerscopedonoff.h"

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetPCBVersion = 0x0005,
};

AtmelEmobCtrl::AtmelEmobCtrl(ZeraMcontrollerIoPtr i2cCtrl, QString devnode, quint8 adrMux, quint8 muxChannel) :
    m_i2cCtrl(i2cCtrl),
    m_i2cMuxer(I2cMultiplexerFactory::createPCA9547Muxer(devnode, adrMux, muxChannel))
{
}

ZeraMControllerIo::atmelRM AtmelEmobCtrl::readCTRLVersion(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    m_i2cCtrl->bootloaderStartProgram();
    return m_i2cCtrl->readVariableLenText(hwGetCtrlVersion, answer);
}

ZeraMControllerIo::atmelRM AtmelEmobCtrl::readPCBVersion(QString &answer)
{
    I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
    m_i2cCtrl->bootloaderStartProgram();
    return m_i2cCtrl->readVariableLenText(hwGetPCBVersion, answer);
}
