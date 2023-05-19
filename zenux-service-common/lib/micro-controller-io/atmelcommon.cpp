#include "atmelcommon.h"

enum hw_cmdcode
{
    hwGetCritStat = 0x0200,
    hwResetCritStat = 0x0201,
    hwSetIntMask = 0x0202,
    hwGetIntMask = 0x0203
};

AtmelCommon::AtmelCommon(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMControllerIo(devnode, adr, debuglevel)
{
}

ZeraMControllerIo::atmelRM AtmelCommon::writeIntMask(quint16 mask)
{
    quint8 PAR[2];
    PAR[0] = (mask >> 8) & 255;
    PAR[1] = mask & 255;
    hw_cmd CMD(hwSetIntMask, 0, PAR, 2);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}

ZeraMControllerIo::atmelRM AtmelCommon::readIntMask(quint16 &mask)
{
    ZeraMControllerIo::atmelRM ret = cmdexecfault;
    quint8 answ[3];
    hw_cmd CMD(hwGetIntMask, 0, nullptr, 0);
    writeCommand(&CMD, answ, 3);
    if(getLastErrorMask() == 0) {
         mask = static_cast<quint16>(answ[0] << 8) + answ[1];
         ret = cmddone;
    }
    return ret;
}

ZeraMControllerIo::atmelRM AtmelCommon::readCriticalStatus(quint16 &stat)
{
    ZeraMControllerIo::atmelRM ret = cmdexecfault;
    quint8 answ[3];
    hw_cmd CMD(hwGetCritStat, 0, nullptr, 0);
    writeCommand(&CMD, answ, 3);
    if(getLastErrorMask() == 0) {
         stat = (static_cast<quint16>(answ[0]) << 8) + answ[1];
         ret = cmddone;
    }
    return ret;
}

ZeraMControllerIo::atmelRM AtmelCommon::resetCriticalStatus(quint16 stat)
{
    quint8 PAR[2];
    PAR[0] = (stat >> 8) & 255;
    PAR[1] = stat & 255;
    hw_cmd CMD(hwResetCritStat, 0, PAR, 2);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}
