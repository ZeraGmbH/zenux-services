#include <syslog.h>
#include <crcutils.h>

#include "mt310s2dglobal.h"
#include "i2cutils.h"
#include "atmelsysctrl.h"

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetPCBVersion = 0x0005,
    hwSetTestModes = 0x0042,
    hwGetAccumulatorStatus = 0x0210,
    hwGetAccumulatorSoc = 0x0211,
};


cATMELSysCtrl::cATMELSysCtrl(QString devnode, quint8 adr, quint8 debuglevel) :
    AtmelCommon(devnode, adr, debuglevel)
{
}


ZeraMcontrollerBase::atmelRM cATMELSysCtrl::readCTRLVersion(QString& answer)
{
    return readVariableLenText(hwGetCtrlVersion, answer);
}

ZeraMcontrollerBase::atmelRM cATMELSysCtrl::readPCBVersion(QString &answer)
{
    return readVariableLenText(hwGetPCBVersion, answer);
}

ZeraMcontrollerBase::atmelRM cATMELSysCtrl::readAccumulatorStatus(quint8 &stat)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD(hwGetAccumulatorStatus, 0, nullptr, 0);
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
         stat = answ[0];
         ret = cmddone;
    }
    return ret;
}

ZeraMcontrollerBase::atmelRM cATMELSysCtrl::readAccumulatorSoc(quint8 &charge)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD(hwGetAccumulatorSoc, 0, nullptr, 0);
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
         charge = answ[0];
         ret = cmddone;
    }
    return ret;

}

ZeraMcontrollerBase::atmelRM cATMELSysCtrl::enableTestMode(qint32 testBits)
{
    quint8 PAR[4];
    PAR[0] = (testBits >> 24) & 255;
    PAR[1] = (testBits >> 16) & 255;
    PAR[2] = (testBits >> 8) & 255;
    PAR[3] = testBits & 255;
    hw_cmd CMD(hwSetTestModes, 0, PAR, 4);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}
