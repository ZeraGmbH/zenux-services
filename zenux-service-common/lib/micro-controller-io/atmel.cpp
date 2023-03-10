#include "i2cutils.h"
#include "atmel.h"
#include "atmelerrorlog.h"
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

QString Atmel::m_devnode = QString();
quint8 Atmel::m_adr = 0;
quint8 Atmel::m_debuglevel = 0;

void Atmel::init(QString devnode, quint8 adr, quint8 debuglevel)
{
    m_devnode = devnode;
    m_adr = adr;
    m_debuglevel = debuglevel;
}

Atmel &Atmel::getInstance() {
    if(m_devnode.isEmpty() && m_adr == 0) {
        syslog(LOG_ERR, "Atmel::getInstance called before init.");
    }
    static Atmel cATMELInstance(m_devnode, m_adr, m_debuglevel);
    return cATMELInstance;
}

Atmel::Atmel(QString devnode, quint8 adr, quint8 debuglevel) :
    AtmelCommon(devnode, adr, debuglevel)
{
    PermissionFunctions::setPermissionPinController(this);
}


ZeraMcontrollerBase::atmelRM Atmel::readSerialNumber(QString& answer)
{
    return readVariableLenText(hwGetSerialNr, answer);
}


ZeraMcontrollerBase::atmelRM Atmel::writeSerialNumber(QString &sNumber)
{
    ZeraMcontrollerBase::atmelRM ret;
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


ZeraMcontrollerBase::atmelRM Atmel::readDeviceName(QString& answer)
{
    return readVariableLenText(hwGetDevName, answer);
}


ZeraMcontrollerBase::atmelRM Atmel::readPCBVersion(QString& answer)
{
    return readVariableLenText(hwGetPCBVersion, answer);
}


ZeraMcontrollerBase::atmelRM Atmel::writePCBVersion(QString &sVersion)
{
    ZeraMcontrollerBase::atmelRM ret;
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


ZeraMcontrollerBase::atmelRM Atmel::readCTRLVersion(QString& answer)
{
    return readVariableLenText(hwGetCtrlVersion, answer);
}


ZeraMcontrollerBase::atmelRM Atmel::readLCAVersion(QString& answer)
{
    return readVariableLenText(hwGetLCAVersion, answer);
}


ZeraMcontrollerBase::atmelRM Atmel::startBootLoader()
{
    hw_cmd CMD(hwStartBootloader, 0, nullptr, 0);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}


ZeraMcontrollerBase::atmelRM Atmel::readChannelStatus(quint8 channel, quint8 &stat)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD (hwGetStatus, channel, nullptr, 0);
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
        stat = answ[0];
        ret = cmddone;
    }
    return ret;
}

ZeraMcontrollerBase::atmelRM Atmel::readClampStatus(quint16 &stat)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD(hwGetClampStatus, 0, nullptr, 0);
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
         stat = answ[0];
         ret = cmddone;
    }
    return ret;
}

ZeraMcontrollerBase::atmelRM Atmel::readRange(quint8 channel, quint8 &range)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
    hw_cmd CMD(hwGetRange, channel, nullptr, 0);
    quint8 answ[2];
    writeCommand(&CMD, answ, 2);
    quint32 errorMask = getLastErrorMask();
    AtmelErrorLog::logReadRange(channel, errorMask);
    if(errorMask == 0) {
        range = answ[0];
        ret = cmddone;
    }
    return ret;
}


ZeraMcontrollerBase::atmelRM Atmel::setRange(quint8 channel, quint8 range)
{
    hw_cmd CMD(hwSetRange, channel, &range, 1);
    writeCommand(&CMD);
    quint32 errorMask = getLastErrorMask();
    AtmelErrorLog::logSetRange(channel, range, errorMask);
    return errorMask == 0 ? cmddone : cmdexecfault;
}


ZeraMcontrollerBase::atmelRM Atmel::getEEPROMAccessEnable(bool &enable)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
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


ZeraMcontrollerBase::atmelRM Atmel::readSamplingRange(quint8 &srange)
{
    srange = 0;
    return cmddone;
}


ZeraMcontrollerBase::atmelRM Atmel::setSamplingRange(quint8)
{
    return cmddone;
}


ZeraMcontrollerBase::atmelRM Atmel::setMeasMode(quint8 mmode)
{
    hw_cmd CMD(hwSetMode, 0, &mmode, 1);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}


ZeraMcontrollerBase::atmelRM Atmel::readMeasMode(quint8 &mmode)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
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


ZeraMcontrollerBase::atmelRM Atmel::setPLLChannel(quint8 chn)
{
    hw_cmd CMD(hwSetPLLChannel, 0, &chn, 1);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}


ZeraMcontrollerBase::atmelRM Atmel::readPLLChannel(quint8& chn)
{
    ZeraMcontrollerBase::atmelRM ret = cmdexecfault;
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