#include <syslog.h>
#include <crcutils.h>

#include "mt310s2dglobal.h"
#include "i2cutils.h"
#include "atmelsysctrl.h"

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
    hwGetAccumulatorStatus = 0x0210,
};


cATMELSysCtrl::cATMELSysCtrl(QString devnode, quint8 adr, quint8 debuglevel) :
    AtmelCommon(devnode, adr, debuglevel)
{
}


ZeraMcontrollerBase::atmelRM cATMELSysCtrl::readCTRLVersion(QString& answer)
{
    return readVariableLenText(hwGetCtrlVersion, answer);
}

ZeraMcontrollerBase::atmelRM cATMELSysCtrl::readAccumulatorStatus(quint16 &stat)
{
    ZeraMcontrollerBase::atmelRM cmdStatus = cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD(hwGetAccumulatorStatus, 0, nullptr, 0);
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
         stat = answ[0];
         cmdStatus = cmddone;
    }
    return cmdStatus;
}
