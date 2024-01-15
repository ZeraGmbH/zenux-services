#include "atmelctrlemobfortest.h"

int AtmelCtrlEmobForTest::m_instanceCount = 0;

AtmelCtrlEmobForTest::AtmelCtrlEmobForTest(ZeraMcontrollerIoPtr i2cCtrl, QString devnode, quint8 adrMux, quint8 muxChannel, bool responding) :
    AtmelCtrlEmob(i2cCtrl, devnode, adrMux, muxChannel),
    m_devnode(devnode),
    m_adrMux(adrMux),
    m_muxChannel(muxChannel),
    m_responding(responding)
{
    m_instanceCount++;
}

AtmelCtrlEmobForTest::~AtmelCtrlEmobForTest()
{
    m_instanceCount--;
}

ZeraMControllerIo::atmelRM AtmelCtrlEmobForTest::readCTRLVersion(QString &answer)
{
    if(m_responding) {
        answer = "EMOB test";
        return ZeraMControllerIo::cmddone;
    }
    else
        return ZeraMControllerIo::cmdexecfault;
}

QString AtmelCtrlEmobForTest::getDevnode()
{
    return m_devnode;
}

void AtmelCtrlEmobForTest::setAdrCtrl(quint8 adrCtrl)
{
    m_adrCtrl = adrCtrl;
}

quint8 AtmelCtrlEmobForTest::getAdrCtrl()
{
    return m_adrCtrl;
}

quint8 AtmelCtrlEmobForTest::getAdrMux()
{
    return m_adrMux;
}

quint8 AtmelCtrlEmobForTest::getMuxChannel()
{
    return m_muxChannel;
}

void AtmelCtrlEmobForTest::setDebuglevel(quint8 level)
{
    m_debuglevel = level;
}

quint8 AtmelCtrlEmobForTest::getDebuglevel()
{
    return m_debuglevel;
}

int AtmelCtrlEmobForTest::getInstanceCount()
{
    return m_instanceCount;
}
