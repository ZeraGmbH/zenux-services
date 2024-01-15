#include "i2cutils.h"
#include "atmel.h"
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

void Atmel::setInstanceParams(QString devnode, quint8 adr, quint8 debuglevel)
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
}


ZeraMControllerIo::atmelRM Atmel::startBootLoader()
{
    hw_cmd CMD(hwStartBootloader, 0, nullptr, 0);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}


ZeraMControllerIo::atmelRM Atmel::readChannelStatus(quint8 channel, quint8 &stat)
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

ZeraMControllerIo::atmelRM Atmel::readClampStatus(quint16 &stat)
{
    ZeraMControllerIo::atmelRM ret = cmdexecfault;
    quint8 answ[2];
    hw_cmd CMD(hwGetClampStatus, 0, nullptr, 0);
    writeCommand(&CMD, answ, 2);
    if(getLastErrorMask() == 0) {
         stat = answ[0];
         ret = cmddone;
    }
    return ret;
}

ZeraMControllerIo::atmelRM Atmel::setPLLChannel(quint8 chn)
{
    hw_cmd CMD(hwSetPLLChannel, 0, &chn, 1);
    writeCommand(&CMD);
    return getLastErrorMask() == 0 ? cmddone : cmdexecfault;
}


ZeraMControllerIo::atmelRM Atmel::readPLLChannel(quint8& chn)
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
