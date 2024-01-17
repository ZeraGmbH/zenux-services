#include "i2cutils.h"
#include "atmelctrlrelais.h"
#include "permissionfunctions.h"
#include <syslog.h>
#include <crcutils.h>

enum hw_cmdcode
{
    hwGetSerialNr = 0x0001,
    hwGetDevName = 0x0002,
    hwGetCtrlVersion = 0x0003,
    hwGetLCAVersion = 0x0004,
    hwGetPCBVersion = 0x0005,
    hwSetSerialNr = 0x0006,
    hwSetPCBVersion = 0x0007,
    hwStartBootloader = 0x0008,

    hwGetInfStat  = 0x0100,

    hwGetClampStatus = 0x0204,

    hwSetSeqMask = 0x1000,
    hwGetSeqMask = 0x1001,
    hwSetFrequency = 0x1006,
    hwGetFrequency = 0x1007,
    hwSetSRate = 0x1008,
    hwGetSRate = 0x1009,
    hwGetPCBTemp = 0x100A,

    hwGetOVRRange = 0x1102,
    hwGetStatus = 0x1103
};

AtmelCtrlRelais::AtmelCtrlRelais(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMControllerIo(devnode, adr, debuglevel)
{
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readPCBVersion(QString& answer)
{
    return readVariableLenText(hwGetPCBVersion, answer);
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readCTRLVersion(QString& answer)
{
    return readVariableLenText(hwGetCtrlVersion, answer);
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readChannelStatus(quint8 channel, quint8 &stat)
{
    ZeraMControllerIo::atmelRM ret = cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD (hwGetStatus, channel, nullptr, 0);
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
        stat = answ[0];
        ret = cmddone;
    }
    return ret;
}

