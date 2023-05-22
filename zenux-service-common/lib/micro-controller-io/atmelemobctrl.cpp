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
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIoTemplate::cmddone;
    if(m_ctrlVersion.isEmpty()) {
        I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
        ret = m_i2cCtrl->readVariableLenText(hwGetCtrlVersion, m_ctrlVersion);
    }
    answer = m_ctrlVersion;
    return ret;
}

ZeraMControllerIo::atmelRM AtmelEmobCtrl::readPCBVersion(QString &answer)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIoTemplate::cmddone;
    if(m_pcbVersion.isEmpty()) {
        I2cMuxerScopedOnOff i2cMuxerEnabled(m_i2cMuxer);
        ret = m_i2cCtrl->readVariableLenText(hwGetPCBVersion, m_pcbVersion);
    }
    answer = m_pcbVersion;
    return ret;
}
