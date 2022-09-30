#include <syslog.h>
#include <crcutils.h>

#include "mt310s2dglobal.h"
#include "i2cutils.h"
#include "atmelsysctrl.h"

enum hw_cmdcode
{
    hwGetCtrlVersion = 0x0003,
};


cATMELSysCtrl::cATMELSysCtrl(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMcontrollerBase(devnode, adr, debuglevel)
{
}


ZeraMcontrollerBase::atmelRM cATMELSysCtrl::readCTRLVersion(QString& answer)
{
    return readVariableLenText(hwGetCtrlVersion, answer);
}
