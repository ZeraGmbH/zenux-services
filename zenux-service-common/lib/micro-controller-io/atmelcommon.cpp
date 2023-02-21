#include "atmelcommon.h"

enum hw_cmdcode
{
    hwSetIntMask = 0x0202,
    hwGetIntMask = 0x0203
};

AtmelCommon::AtmelCommon(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMcontrollerBase(devnode, adr, debuglevel)
{
}

ZeraMcontrollerBase::atmelRM AtmelCommon::writeIntMask(quint16 mask)
{
    quint8 PAR[2];
    PAR[0] = (mask >> 8) & 255;
    PAR[1] = mask & 255;
    hw_cmd CMD(hwSetIntMask, 0, PAR, 2);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}


ZeraMcontrollerBase::atmelRM AtmelCommon::readIntMask(quint16 &mask)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
    quint8 answ[3];
    hw_cmd CMD(hwGetIntMask, 0, nullptr, 0);
    writeCommand(&CMD, answ, 3);
    if(getLastErrorMask() == 0) {
         mask = static_cast<quint16>(answ[0] << 8) + answ[1];
         ret = cmddone;
    }
    return ret;
}
