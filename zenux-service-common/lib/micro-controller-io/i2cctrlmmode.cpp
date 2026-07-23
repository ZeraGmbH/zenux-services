#include "i2cctrlmmode.h"

I2cCtrlMMode::I2cCtrlMMode(QString deviceNodeName, quint8 i2cAddress, quint8 debugLevel) :
    m_ctrlIo(deviceNodeName, i2cAddress, debugLevel)
{
}

/* mmode COM5003:
    enum enMeasurementModes
    {
        MEAS_MODE_NORMAL = 0,
        MEAS_MODE_0V_ABGL,
        MEAS_MODE_1V_REF,

        MEAS_MODE_COUNT
    };
 see https://github.com/ZeraGmbH/atmel-com5003/blob/1664607ebdd8938c479f182c48bac8afe901918c/Control_Relais.h#L17
 */

enum hw_cmdcode
{
    hwSetMode = 0x1004,
    hwGetMode = 0x1005,
};

ZeraMControllerIoTemplate::atmelRM I2cCtrlMMode::setMeasMode(quint8 mmode)
{
    hw_cmd CMD(hwSetMode, 0, &mmode, 1);
    m_ctrlIo.writeCommand(&CMD);
    return m_ctrlIo.getLastErrorMask() == 0 ? ZeraMControllerIo::cmddone : ZeraMControllerIo::cmdexecfault;
}

ZeraMControllerIoTemplate::atmelRM I2cCtrlMMode::readMeasMode(quint8 &mmode)
{
    ZeraMControllerIo::atmelRM ret = ZeraMControllerIo::cmdexecfault;
    mmode = 0; // default AC
    hw_cmd CMD(hwGetMode, 0, nullptr, 0);
    quint8 answ[2];
    m_ctrlIo.writeCommand(&CMD, answ, 2);
    if(m_ctrlIo.getLastErrorMask() == 0) {
        mmode = answ[0];
        ret = ZeraMControllerIo::cmddone;
    }
    return ret;
}
