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
    hwSetPLLChannel = 0x1002,
    hwGetPLLChannel = 0x1003,
    hwSetMode = 0x1004,
    hwGetMode = 0x1005,
    hwSetFrequency = 0x1006,
    hwGetFrequency = 0x1007,
    hwSetSRate = 0x1008,
    hwGetSRate = 0x1009,
    hwGetPCBTemp = 0x100A,
    hwGetFlashWriteAccess=0x100B,

    hwSetRange = 0x1100,
    hwGetRange = 0x1101,
    hwGetOVRRange = 0x1102,
    hwGetStatus = 0x1103
};

AtmelCtrlRelais::AtmelCtrlRelais(QString devnode, quint8 adr, quint8 debuglevel) :
    ZeraMControllerIo(devnode, adr, debuglevel)
{
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readSerialNumber(QString& answer)
{
    return readVariableLenText(hwGetSerialNr, answer);
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::writeSerialNumber(QString &sNumber)
{
    ZeraMControllerIo::atmelRM ret;
    quint16 len = static_cast<quint16>(sNumber.length());
    if ( (len<1) || (len>24)) {
        ret = cmdfault;
    }
    else {
        QByteArray ba = sNumber.toLatin1();
        hw_cmd CMD(hwSetSerialNr, 0, reinterpret_cast<quint8*>(ba.data()), len);
        writeCommand(&CMD);
        ret = getLastErrorMask() == 0 ? cmddone : cmdexecfault;
    }
    return ret;
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readDeviceName(QString& answer)
{
    return readVariableLenText(hwGetDevName, answer);
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readPCBVersion(QString& answer)
{
    return readVariableLenText(hwGetPCBVersion, answer);
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::writePCBVersion(QString &sVersion)
{
    ZeraMControllerIo::atmelRM ret;
    quint16 len = static_cast<quint16>(sVersion.length());
    if (len<1 || len>24) {
        ret = cmdfault;
    }
    else {
        QByteArray ba = sVersion.toLatin1();
        hw_cmd CMD(hwSetPCBVersion, 0, reinterpret_cast<quint8*>(ba.data()), len);
        writeCommand(&CMD);
        ret = getLastErrorMask() == 0 ? cmddone : cmdexecfault;
    }
    return ret;
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readCTRLVersion(QString& answer)
{
    return readVariableLenText(hwGetCtrlVersion, answer);
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readLCAVersion(QString& answer)
{
    return readVariableLenText(hwGetLCAVersion, answer);
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::startBootLoader()
{
    hw_cmd CMD(hwStartBootloader, 0, nullptr, 0);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
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

ZeraMControllerIo::atmelRM AtmelCtrlRelais::readRange(quint8 channel, quint8 &range)
{
    hw_cmd CMD(hwGetRange, channel, nullptr, 0);
    quint8 answ[2];
    writeCommand(&CMD, answ, 2);
    quint32 errorMask = getLastErrorMask();
    ZeraMControllerIo::atmelRM ret = errorMask == 0 ? cmddone : cmdexecfault;
    if(ret == cmddone)
        range = answ[0];
    else
        qWarning("ReadRange failed with ch: %i / mask: %8X failed!",
                 channel, errorMask);
    return ret;
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::setRange(quint8 channel, quint8 range)
{
    hw_cmd CMD(hwSetRange, channel, &range, 1);
    writeCommand(&CMD);
    quint32 errorMask = getLastErrorMask();
    ZeraMControllerIo::atmelRM ret = errorMask == 0 ? cmddone : cmdexecfault;
    if(ret != cmddone)
        qWarning("SetRange failed with ch: %i / rng: %i / mask: %8X failed!",
                 channel, range, errorMask);
    return ret;
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::getEEPROMAccessEnable(bool &enable)
{
    ZeraMControllerIo::atmelRM ret = cmdexecfault;
    enable = false; // default
    hw_cmd CMD(hwGetFlashWriteAccess, 0, nullptr, 0);
    quint8 answ[2];
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
        enable = (answ[0] != 0);
        ret = cmddone;
    }
    return ret;
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::setMeasMode(quint8 mmode)
{
    hw_cmd CMD(hwSetMode, 0, &mmode, 1);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readMeasMode(quint8 &mmode)
{
    ZeraMControllerIo::atmelRM ret = cmdexecfault;
    mmode = 0; // default AC
    hw_cmd CMD(hwGetMode, 0, nullptr, 0);
    quint8 answ[2];
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
        mmode = answ[0];
        ret = cmddone;
    }
    return ret;
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::setPLLChannel(quint8 chn)
{
    hw_cmd CMD(hwSetPLLChannel, 0, &chn, 1);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}


ZeraMControllerIo::atmelRM AtmelCtrlRelais::readPLLChannel(quint8& chn)
{
    ZeraMControllerIo::atmelRM ret = cmdexecfault;
    chn = 0; // default AC
    hw_cmd CMD(hwGetPLLChannel, 0, nullptr, 0);
    quint8 answ[2];
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
        chn = answ[0];
        ret = cmddone;
    }
    return ret;
}
